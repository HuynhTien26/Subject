<?php
header('Content-Type: application/json');
require_once 'db.php';

try {
    // Lấy tất cả đơn hàng cùng chi tiết, bao gồm DaThuTien
    $sql = "
        SELECT 
            d.MaDon AS id,
            DATE(d.NgayBan) AS date,
            k.TenKhach AS customer,
            COALESCE(SUM(c.SoLuong), 0) AS qty,
            COALESCE(SUM(c.SoLuong * h.DonGia), 0) AS cost,
            COALESCE(SUM(c.SoLuong * c.DonGia), 0) AS revenue,
            d.DaThuTien AS paid_status
        FROM DONHANG d
        LEFT JOIN KHACHHANG k ON d.MaKhach = k.MaKhach
        LEFT JOIN CHITIETDON c ON d.MaDon = c.MaDon
        LEFT JOIN HANGHOA h ON c.MaHang = h.MaHang
        GROUP BY d.MaDon
        ORDER BY d.NgayBan DESC
    ";
    $stmt = $conn->prepare($sql);
    $stmt->execute();
    $orders = $stmt->fetchAll(PDO::FETCH_ASSOC);

    // Tính paid dựa trên DaThuTien (1 = đã thu, 0 = chưa thu)
    foreach ($orders as &$order) {
        $order['paid'] = ($order['paid_status'] == 1) ? $order['revenue'] : 0;
        unset($order['paid_status']);
    }

    echo json_encode($orders);
} catch (Exception $e) {
    echo json_encode(['error' => $e->getMessage()]);
}
?>