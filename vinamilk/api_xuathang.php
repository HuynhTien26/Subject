<?php
header('Content-Type: application/json');
require_once 'db.php';

$action = isset($_GET['action']) ? $_GET['action'] : '';

try {
    switch ($action) {
        case 'get_products':
            getProducts();
            break;
        case 'get_customers':
            getCustomers();
            break;
        case 'get_orders':
            getOrders();
            break;
        case 'get_order_details':
            getOrderDetails();
            break;
        case 'create_order':
            createOrder();
            break;
        case 'update_order_status':
            updateOrderStatus();
            break;
        case 'update_payment_status':
            updatePaymentStatus();
            break;
        default:
            echo json_encode(['error' => 'Action không hợp lệ']);
            break;
    }
} catch (Exception $e) {
    echo json_encode(['error' => $e->getMessage()]);
}

function updatePaymentStatus() {
    global $conn;
    $input = json_decode(file_get_contents('php://input'), true);
    if (!$input || !isset($input['orderId'])) {
        echo json_encode(['error' => 'Thiếu mã đơn']);
        return;
    }
    $orderId = intval($input['orderId']);
    $paid = isset($input['paid']) ? intval($input['paid']) : 1; // mặc định đã thu

    $sql = "UPDATE DONHANG SET DaThuTien = :paid WHERE MaDon = :maDon";
    $stmt = $conn->prepare($sql);
    $stmt->execute([':paid' => $paid, ':maDon' => $orderId]);

    echo json_encode(['success' => true]);
}
function getProducts() {
    global $conn;
    $stmt = $conn->query("SELECT MaHang, TenHang, SoLuongTonKho, DonGia, DonViTinh FROM HANGHOA ORDER BY TenHang");
    $products = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($products);
}

function getCustomers() {
    global $conn;
    $stmt = $conn->query("SELECT MaKhach, TenKhach FROM KHACHHANG ORDER BY TenKhach");
    $customers = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($customers);
}

function getOrders() {
    global $conn;
    $statusFilter = isset($_GET['status']) ? $_GET['status'] : '';
    $sql = "SELECT d.MaDon, d.NgayBan, d.MaKhach, k.TenKhach, d.TongTien, d.TrangThai, d.DaThuTien
            FROM DONHANG d
            JOIN KHACHHANG k ON d.MaKhach = k.MaKhach";

    if ($statusFilter !== '' && $statusFilter !== 'all') {
        $dbStatus = mapStatusToDB($statusFilter);
        $sql .= " WHERE d.TrangThai = :status";
        $stmt = $conn->prepare($sql);
        $stmt->execute([':status' => $dbStatus]);
    } else {
        $stmt = $conn->prepare($sql);
        $stmt->execute();
    }
    $orders = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($orders);
}

function getOrderDetails() {
    global $conn;
    $orderId = isset($_GET['id']) ? intval($_GET['id']) : 0;
    if ($orderId <= 0) {
        echo json_encode(['error' => 'Mã đơn không hợp lệ']);
        return;
    }
    $sql = "SELECT c.MaHang, h.TenHang, c.SoLuong, c.DonGia, (c.SoLuong * c.DonGia) AS ThanhTien
            FROM CHITIETDON c
            JOIN HANGHOA h ON c.MaHang = h.MaHang
            WHERE c.MaDon = :maDon";
    $stmt = $conn->prepare($sql);
    $stmt->execute([':maDon' => $orderId]);
    $details = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($details);
}

function createOrder() {
    global $conn;
    $input = json_decode(file_get_contents('php://input'), true);
    if (!$input || !isset($input['customerId']) || !isset($input['date']) || empty($input['items'])) {
        echo json_encode(['error' => 'Thiếu thông tin đơn hàng']);
        return;
    }

    $customerId = intval($input['customerId']);
    $ngayBan = $input['date'];
    $items = $input['items']; // mỗi item có productId, quantity

    $conn->beginTransaction();
    try {
        // Kiểm tra tồn kho
        $stockErrors = [];
        foreach ($items as $item) {
            $productId = intval($item['productId']);
            $quantity = intval($item['quantity']);
            if ($quantity <= 0) {
                $stockErrors[] = "Số lượng không hợp lệ cho sản phẩm ID $productId";
                continue;
            }
            $stmt = $conn->prepare("SELECT SoLuongTonKho, TenHang FROM HANGHOA WHERE MaHang = :id");
            $stmt->execute([':id' => $productId]);
            $product = $stmt->fetch(PDO::FETCH_ASSOC);
            if (!$product) {
                $stockErrors[] = "Sản phẩm ID $productId không tồn tại";
            } elseif ($product['SoLuongTonKho'] < $quantity) {
                $stockErrors[] = "Sản phẩm {$product['TenHang']} không đủ tồn kho (còn {$product['SoLuongTonKho']}, yêu cầu $quantity)";
            }
        }
        if (!empty($stockErrors)) {
            throw new Exception(implode('; ', $stockErrors));
        }

        // Tạo đơn hàng
        $sqlInsertDon = "INSERT INTO DONHANG (NgayBan, MaKhach, TrangThai, DaThuTien) VALUES (:ngay, :maKhach, 'Đang xử lý', 0)";
        $stmt = $conn->prepare($sqlInsertDon);
        $stmt->execute([':ngay' => $ngayBan, ':maKhach' => $customerId]);
        $maDon = $conn->lastInsertId();

        // Thêm chi tiết và trừ kho
        $sqlInsertChiTiet = "INSERT INTO CHITIETDON (MaDon, MaHang, SoLuong, DonGia) VALUES (:maDon, :maHang, :soLuong, (SELECT DonGia FROM HANGHOA WHERE MaHang = :maHang))";
        $stmtDetail = $conn->prepare($sqlInsertChiTiet);
        $sqlUpdateTon = "UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho - :soLuong WHERE MaHang = :maHang";
        $stmtUpdate = $conn->prepare($sqlUpdateTon);
        $tongTien = 0;
        foreach ($items as $item) {
            $productId = intval($item['productId']);
            $quantity = intval($item['quantity']);
            $stmtDetail->execute([
                ':maDon' => $maDon,
                ':maHang' => $productId,
                ':soLuong' => $quantity
            ]);
            $stmtUpdate->execute([
                ':soLuong' => $quantity,
                ':maHang' => $productId
            ]);
            // Lấy giá để tính tổng tiền
            $stmtPrice = $conn->prepare("SELECT DonGia FROM HANGHOA WHERE MaHang = :id");
            $stmtPrice->execute([':id' => $productId]);
            $price = $stmtPrice->fetchColumn();
            $tongTien += $quantity * $price;
        }

        // Cập nhật tổng tiền đơn hàng
        $sqlUpdateTong = "UPDATE DONHANG SET TongTien = :tongTien WHERE MaDon = :maDon";
        $stmtTong = $conn->prepare($sqlUpdateTong);
        $stmtTong->execute([':tongTien' => $tongTien, ':maDon' => $maDon]);

        $conn->commit();
        echo json_encode(['success' => true, 'orderId' => $maDon]);
    } catch (Exception $e) {
        $conn->rollBack();
        echo json_encode(['error' => $e->getMessage()]);
    }
}

function updateOrderStatus() {
    global $conn;
    $input = json_decode(file_get_contents('php://input'), true);
    if (!$input || !isset($input['orderId']) || !isset($input['status'])) {
        echo json_encode(['error' => 'Thiếu thông tin']);
        return;
    }
    $orderId = intval($input['orderId']);
    $frontendStatus = $input['status'];
    $dbStatus = mapStatusToDB($frontendStatus);
    if ($dbStatus === null) {
        echo json_encode(['error' => 'Trạng thái không hợp lệ']);
        return;
    }
    $sql = "UPDATE DONHANG SET TrangThai = :status WHERE MaDon = :maDon";
    $stmt = $conn->prepare($sql);
    $stmt->execute([':status' => $dbStatus, ':maDon' => $orderId]);
    echo json_encode(['success' => true]);
}

function mapStatusToDB($frontendStatus) {
    switch ($frontendStatus) {
        case 'processing': return 'Đang xử lý';
        case 'shipping': return 'Đang giao';
        case 'delivered': return 'Đã hoàn thành';
        default: return null;
    }
}
?>