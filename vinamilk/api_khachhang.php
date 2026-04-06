<?php
header('Content-Type: application/json');
require_once 'db.php';

$action = isset($_GET['action']) ? $_GET['action'] : '';

try {
    switch ($action) {
        case 'get_customers':
            getCustomers();
            break;
        case 'add_customer':
            addCustomer();
            break;
        case 'update_customer':
            updateCustomer();
            break;
        default:
            echo json_encode(['error' => 'Action không hợp lệ']);
            break;
    }
} catch (Exception $e) {
    echo json_encode(['error' => $e->getMessage()]);
}

// ===================== LẤY DANH SÁCH KHÁCH HÀNG =====================
function getCustomers() {
    global $conn;
    $sql = "SELECT MaKhach, TenKhach, SoDienThoai, DiaChi FROM KHACHHANG ORDER BY MaKhach";
    $stmt = $conn->prepare($sql);
    $stmt->execute();
    $customers = $stmt->fetchAll(PDO::FETCH_ASSOC);

    // Format dữ liệu phù hợp với frontend (thêm trường note rỗng vì frontend vẫn dùng)
    $result = [];
    foreach ($customers as $c) {
        $result[] = [
            'id' => str_pad($c['MaKhach'], 3, '0', STR_PAD_LEFT), // tạo mã KHxxx
            'name' => $c['TenKhach'],
            'phone' => $c['SoDienThoai'],
            'address' => $c['DiaChi'] ?? '',
            'note' => ''  // không còn cột ghi chú trong DB
        ];
    }
    echo json_encode($result);
}

// ===================== THÊM KHÁCH HÀNG MỚI =====================
function addCustomer() {
    global $conn;
    $input = json_decode(file_get_contents('php://input'), true);
    if (!$input) {
        echo json_encode(['error' => 'Dữ liệu không hợp lệ']);
        return;
    }

    $name = trim($input['name'] ?? '');
    $phone = trim($input['phone'] ?? '');
    $address = trim($input['address'] ?? '');

    if (empty($name) || empty($phone)) {
        echo json_encode(['error' => 'Tên và số điện thoại không được để trống']);
        return;
    }

    $sql = "INSERT INTO KHACHHANG (TenKhach, SoDienThoai, DiaChi) VALUES (:name, :phone, :address)";
    $stmt = $conn->prepare($sql);
    $stmt->execute([
        ':name' => $name,
        ':phone' => $phone,
        ':address' => $address
    ]);

    $newId = $conn->lastInsertId();
    echo json_encode(['success' => true, 'id' => $newId]);
}

// ===================== CẬP NHẬT KHÁCH HÀNG =====================
function updateCustomer() {
    global $conn;
    $input = json_decode(file_get_contents('php://input'), true);
    if (!$input || !isset($input['id'])) {
        echo json_encode(['error' => 'Thiếu mã khách hàng']);
        return;
    }

    $id = intval($input['id']);          // nhận id dạng số
    $name = trim($input['name'] ?? '');
    $phone = trim($input['phone'] ?? '');
    $address = trim($input['address'] ?? '');

    if (empty($name) || empty($phone)) {
        echo json_encode(['error' => 'Tên và số điện thoại không được để trống']);
        return;
    }

    $sql = "UPDATE KHACHHANG SET TenKhach = :name, SoDienThoai = :phone, DiaChi = :address WHERE MaKhach = :id";
    $stmt = $conn->prepare($sql);
    $stmt->execute([
        ':name' => $name,
        ':phone' => $phone,
        ':address' => $address,
        ':id' => $id
    ]);

    echo json_encode(['success' => true]);
}
?>