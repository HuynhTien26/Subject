<?php
// api_khohang.php
header('Content-Type: application/json; charset=utf-8');
require 'db.php'; // Sử dụng lại file kết nối db.php đã có sẵn

try {
    // Câu lệnh lấy dữ liệu và đổi tên cột cho khớp với code Javascript của bạn
    $sql = "
        SELECT 
            CONCAT('SP', LPAD(MaHang, 3, '0')) as id,
            TenHang as name,
            DonViTinh as unit,
            SoLuongTonKho as stock,
            DATE_FORMAT(HanSuDung, '%Y-%m-%d') as expiryDate,
            DonGia as price
        FROM HANGHOA
        ORDER BY TenHang ASC
    ";
    
    $stmt = $conn->query($sql);
    $result = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    // Ép kiểu số
    foreach ($result as &$row) {
        $row['stock'] = (int)$row['stock'];
        $row['price'] = (float)$row['price'];
    }

    echo json_encode($result);

} catch(PDOException $e) {
    echo json_encode(["error" => "Lỗi truy vấn: " . $e->getMessage()]);
}
?>