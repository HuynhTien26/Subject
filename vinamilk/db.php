<?php
// db.php - KẾT NỐI BẰNG MYSQL
$host = "localhost";
$dbname = "QuanLyDLVinamilk";
$username = "root"; // XAMPP mặc định luôn là root
$password = "";     // XAMPP mặc định bỏ trống mật khẩu

try {
    // Kết nối bằng MySQL PDO, thiết lập luôn UTF8 để không bị lỗi font tiếng Việt
    $conn = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8mb4", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch(PDOException $e) {
    die(json_encode(["error" => "Lỗi kết nối MySQL: " . $e->getMessage()]));
}
?>