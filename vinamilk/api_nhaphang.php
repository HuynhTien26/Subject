<?php
header('Content-Type: application/json');
require_once 'db.php'; // kết nối PDO

// Lấy action từ query string
$action = isset($_GET['action']) ? $_GET['action'] : '';

try {
    switch ($action) {
        case 'get_orders':
            getOrders();
            break;
        case 'get_products':
            getProducts();
            break;
        case 'get_order_details':
            getOrderDetails();
            break;
        case 'create_order':
            createOrder();
            break;
        case 'confirm_order':
            confirmOrder();
            break;
        default:
            echo json_encode(['error' => 'Action không hợp lệ']);
            break;
    }
} catch (Exception $e) {
    echo json_encode(['error' => $e->getMessage()]);
}

// ============================
// 1. Lấy danh sách phiếu nhập
// ============================
function getOrders() {
    global $conn;
    $sql = "SELECT p.MaNhap, p.NgayNhap, p.NhaCungCap, p.TrangThai,
                   COALESCE(SUM(c.SoLuong), 0) AS TongSoLuong
            FROM PHIEUNHAP p
            LEFT JOIN CHITIETPHIEUNHAP c ON p.MaNhap = c.MaNhap
            GROUP BY p.MaNhap, p.NgayNhap, p.NhaCungCap, p.TrangThai
            ORDER BY p.NgayNhap DESC";
    $stmt = $conn->prepare($sql);
    $stmt->execute();
    $orders = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($orders);
}

// ============================
// 2. Lấy danh sách sản phẩm cho dropdown
// ============================
function getProducts() {
    global $conn;
    $sql = "SELECT MaHang, TenHang, DonViTinh FROM HANGHOA ORDER BY TenHang";
    $stmt = $conn->prepare($sql);
    $stmt->execute();
    $products = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($products);
}

// ============================
// 3. Lấy chi tiết một phiếu nhập (để xem)
// ============================
function getOrderDetails() {
    global $conn;
    $maNhap = isset($_GET['id']) ? intval($_GET['id']) : 0;
    if ($maNhap <= 0) {
        echo json_encode(['error' => 'Mã phiếu không hợp lệ']);
        return;
    }
    $sql = "SELECT c.MaHang, h.TenHang, h.DonViTinh, c.SoLuong
            FROM CHITIETPHIEUNHAP c
            JOIN HANGHOA h ON c.MaHang = h.MaHang
            WHERE c.MaNhap = :maNhap";
    $stmt = $conn->prepare($sql);
    $stmt->execute([':maNhap' => $maNhap]);
    $details = $stmt->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($details);
}

// ============================
// 4. Tạo phiếu nhập mới (dự trù)
// ============================
function createOrder() {
    global $conn;
    $input = json_decode(file_get_contents('php://input'), true);
    if (!$input || empty($input['supplier']) || empty($input['date']) || empty($input['items'])) {
        echo json_encode(['error' => 'Thiếu thông tin phiếu nhập']);
        return;
    }

    $supplier = $input['supplier'];
    $ngayNhap = $input['date'];
    $items = $input['items']; // mảng [ {id, qty}, ... ]

    // Bắt đầu transaction
    $conn->beginTransaction();
    try {
        // 1. Tạo phiếu nhập với trạng thái 'Đang chờ'
        $sqlInsertPhieu = "INSERT INTO PHIEUNHAP (NgayNhap, NhaCungCap, TrangThai) VALUES (:ngay, :supplier, 'Đang chờ')";
        $stmt = $conn->prepare($sqlInsertPhieu);
        $stmt->execute([':ngay' => $ngayNhap, ':supplier' => $supplier]);
        $maNhap = $conn->lastInsertId();

        // 2. Thêm chi tiết phiếu nhập
        $sqlInsertChiTiet = "INSERT INTO CHITIETPHIEUNHAP (MaNhap, MaHang, SoLuong) VALUES (:maNhap, :maHang, :soLuong)";
        $stmtDetail = $conn->prepare($sqlInsertChiTiet);
        foreach ($items as $item) {
            $stmtDetail->execute([
                ':maNhap' => $maNhap,
                ':maHang' => $item['id'],
                ':soLuong' => $item['qty']
            ]);
        }

        $conn->commit();
        echo json_encode(['success' => true, 'maNhap' => $maNhap]);
    } catch (Exception $e) {
        $conn->rollBack();
        echo json_encode(['error' => 'Lỗi khi lưu phiếu: ' . $e->getMessage()]);
    }
}

// ============================
// 5. Xác nhận phiếu (cập nhật trạng thái + cộng kho)
// ============================
function confirmOrder() {
    global $conn;
    $maNhap = isset($_POST['maNhap']) ? intval($_POST['maNhap']) : 0;
    if ($maNhap <= 0) {
        echo json_encode(['error' => 'Mã phiếu không hợp lệ']);
        return;
    }

    // Bắt đầu transaction
    $conn->beginTransaction();
    try {
        // 1. Kiểm tra phiếu đã được xác nhận chưa
        $sqlCheck = "SELECT TrangThai FROM PHIEUNHAP WHERE MaNhap = :maNhap";
        $stmtCheck = $conn->prepare($sqlCheck);
        $stmtCheck->execute([':maNhap' => $maNhap]);
        $order = $stmtCheck->fetch(PDO::FETCH_ASSOC);
        if (!$order) {
            throw new Exception('Phiếu không tồn tại');
        }
        if ($order['TrangThai'] !== 'Đang chờ') {
            throw new Exception('Phiếu đã được xử lý trước đó');
        }

        // 2. Lấy chi tiết phiếu để cập nhật tồn kho
        $sqlDetail = "SELECT MaHang, SoLuong FROM CHITIETPHIEUNHAP WHERE MaNhap = :maNhap";
        $stmtDetail = $conn->prepare($sqlDetail);
        $stmtDetail->execute([':maNhap' => $maNhap]);
        $details = $stmtDetail->fetchAll(PDO::FETCH_ASSOC);

        if (empty($details)) {
            throw new Exception('Phiếu không có chi tiết mặt hàng');
        }

        // 3. Cộng tồn kho cho từng mặt hàng
        $sqlUpdateTon = "UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho + :soLuong WHERE MaHang = :maHang";
        $stmtUpdate = $conn->prepare($sqlUpdateTon);
        foreach ($details as $item) {
            $stmtUpdate->execute([
                ':soLuong' => $item['SoLuong'],
                ':maHang' => $item['MaHang']
            ]);
        }

        // 4. Cập nhật trạng thái phiếu thành 'Đã hoàn thành'
        $sqlUpdateStatus = "UPDATE PHIEUNHAP SET TrangThai = 'Đã hoàn thành' WHERE MaNhap = :maNhap";
        $stmtStatus = $conn->prepare($sqlUpdateStatus);
        $stmtStatus->execute([':maNhap' => $maNhap]);

        $conn->commit();
        echo json_encode(['success' => true]);
    } catch (Exception $e) {
        $conn->rollBack();
        echo json_encode(['error' => $e->getMessage()]);
    }
}
?>