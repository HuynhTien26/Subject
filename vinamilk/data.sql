-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Apr 01, 2026 at 12:20 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.0.30

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `quanlydlvinamilk`
--

-- --------------------------------------------------------

--
-- Table structure for table `chitietdon`
--

CREATE TABLE `chitietdon` (
  `MaDon` int(11) NOT NULL,
  `MaHang` int(11) NOT NULL,
  `SoLuong` int(11) DEFAULT NULL,
  `DonGia` decimal(18,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `chitietdon`
--

INSERT INTO `chitietdon` (`MaDon`, `MaHang`, `SoLuong`, `DonGia`) VALUES
(1, 1, 15, 320000.00),
(1, 2, 8, 320000.00),
(2, 3, 10, 320000.00),
(3, 4, 20, 330000.00),
(3, 5, 5, 330000.00),
(4, 6, 2, 255000.00),
(5, 7, 12, 265000.00),
(6, 8, 6, 275000.00),
(6, 9, 4, 285000.00),
(7, 10, 3, 350000.00),
(8, 11, 25, 28000.00),
(9, 12, 8, 28000.00),
(10, 13, 10, 32000.00),
(10, 14, 6, 25000.00),
(11, 15, 4, 25000.00),
(12, 16, 7, 22000.00),
(12, 17, 3, 18000.00),
(13, 49, 10, 10000.00),
(14, 49, 10, 10000.00),
(15, 29, 1, 450000.00),
(16, 10, 3, 350000.00),
(17, 23, 10, 320000.00),
(18, 49, 1, 10000.00),
(19, 24, 3, 290000.00),
(20, 24, 3, 290000.00),
(20, 33, 4, 310000.00),
(21, 40, 10, 220000.00),
(22, 45, 10, 55000.00),
(23, 49, 10, 10000.00);

-- --------------------------------------------------------

--
-- Table structure for table `chitietphieunhap`
--

CREATE TABLE `chitietphieunhap` (
  `MaNhap` int(11) NOT NULL,
  `MaHang` int(11) NOT NULL,
  `SoLuong` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `chitietphieunhap`
--

INSERT INTO `chitietphieunhap` (`MaNhap`, `MaHang`, `SoLuong`) VALUES
(1, 49, 10),
(2, 30, 5),
(3, 49, 10),
(4, 36, 1),
(5, 1, 10),
(5, 2, 5),
(5, 3, 8),
(6, 4, 12),
(6, 5, 6),
(7, 10, 3),
(7, 11, 20),
(8, 15, 7),
(8, 16, 4),
(9, 30, 10),
(9, 49, 10);

-- --------------------------------------------------------

--
-- Table structure for table `donhang`
--

CREATE TABLE `donhang` (
  `MaDon` int(11) NOT NULL,
  `NgayBan` datetime DEFAULT current_timestamp(),
  `MaKhach` int(11) DEFAULT NULL,
  `TongTien` decimal(18,2) DEFAULT 0.00,
  `TrangThai` varchar(50) DEFAULT NULL,
  `DaThuTien` tinyint(1) DEFAULT 0 COMMENT '0: chưa thu, 1: đã thu'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `donhang`
--

INSERT INTO `donhang` (`MaDon`, `NgayBan`, `MaKhach`, `TongTien`, `TrangThai`, `DaThuTien`) VALUES
(1, '2026-01-10 08:00:00', 1, 7360000.00, 'Đã hoàn thành', 1),
(2, '2026-01-15 10:30:00', 2, 3200000.00, 'Đang giao', 1),
(3, '2026-01-20 14:00:00', 3, 8250000.00, 'Đang giao', 1),
(4, '2026-01-25 16:45:00', 4, 510000.00, 'Đã hoàn thành', 1),
(5, '2026-02-05 09:15:00', 5, 3180000.00, 'Đang giao', 1),
(6, '2026-02-12 11:20:00', 6, 2790000.00, 'Đã hoàn thành', 1),
(7, '2026-02-18 13:30:00', 7, 1050000.00, 'Đang giao', 0),
(8, '2026-02-25 15:00:00', 8, 700000.00, 'Đã hoàn thành', 1),
(9, '2026-03-05 09:00:00', 9, 224000.00, 'Đang giao', 0),
(10, '2026-03-12 10:15:00', 10, 470000.00, 'Đang giao', 0),
(11, '2026-03-18 14:30:00', 11, 100000.00, 'Đã hoàn thành', 1),
(12, '2026-03-25 16:20:00', 12, 208000.00, 'Đang giao', 0),
(13, '2026-03-25 00:00:00', 12, 100000.00, 'Đang giao', 0),
(14, '2026-03-30 00:00:00', 17, 100000.00, 'Đang giao', 0),
(15, '2026-03-25 00:00:00', 12, 450000.00, 'Đã hoàn thành', 0),
(16, '2026-03-31 00:00:00', 14, 1050000.00, 'Đang giao', 0),
(17, '2026-03-31 00:00:00', 14, 3200000.00, 'Đang giao', 1),
(18, '2026-03-31 00:00:00', 9, 10000.00, 'Đã hoàn thành', 1),
(19, '2026-03-31 00:00:00', 20, 870000.00, 'Đã hoàn thành', 0),
(20, '2026-03-31 00:00:00', 12, 2110000.00, 'Đã hoàn thành', 1),
(21, '2026-03-31 00:00:00', 12, 2200000.00, 'Đang xử lý', 1),
(22, '2026-03-31 00:00:00', 9, 550000.00, 'Đã hoàn thành', 0),
(23, '2026-03-31 00:00:00', 12, 100000.00, 'Đang xử lý', 0);

-- --------------------------------------------------------

--
-- Table structure for table `hanghoa`
--

CREATE TABLE `hanghoa` (
  `MaHang` int(11) NOT NULL,
  `TenHang` varchar(100) NOT NULL,
  `DonViTinh` varchar(20) DEFAULT NULL,
  `SoLuongTonKho` int(11) DEFAULT 0,
  `HanSuDung` date DEFAULT NULL,
  `DonGia` decimal(18,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `hanghoa`
--

INSERT INTO `hanghoa` (`MaHang`, `TenHang`, `DonViTinh`, `SoLuongTonKho`, `HanSuDung`, `DonGia`) VALUES
(1, 'Sữa tươi Vinamilk Có đường 180ml', 'Thùng', 450, '2026-12-01', 320000.00),
(2, 'Sữa tươi Vinamilk Ít đường 180ml', 'Thùng', 424, '2026-12-15', 320000.00),
(3, 'Sữa tươi Vinamilk Không đường 180ml', 'Thùng', 264, '2026-11-20', 320000.00),
(4, 'Sữa tươi Vinamilk Dâu 180ml', 'Thùng', 136, '2026-10-10', 330000.00),
(5, 'Sữa tươi Vinamilk Socola 180ml', 'Thùng', 228, '2026-10-05', 330000.00),
(6, 'Sữa bột Dielac Alpha Gold 1', 'Hộp', 42, '2026-05-20', 255000.00),
(7, 'Sữa bột Dielac Alpha Gold 2', 'Hộp', 15, '2026-06-15', 265000.00),
(8, 'Sữa bột Dielac Alpha Gold 3', 'Hộp', 20, '2026-07-10', 275000.00),
(9, 'Sữa bột Dielac Alpha Gold 4', 'Hộp', 25, '2026-08-05', 285000.00),
(10, 'Sữa bột Optimum Gold 1', 'Hộp', 20, '2026-05-12', 350000.00),
(11, 'Sữa chua Vinamilk Có đường', 'Lốc', 970, '2026-04-15', 28000.00),
(12, 'Sữa chua Vinamilk Ít đường', 'Lốc', 784, '2026-04-20', 28000.00),
(13, 'Sữa chua Vinamilk Nha đam', 'Lốc', 580, '2026-04-23', 32000.00),
(14, 'Sữa chua uống Probi Dâu', 'Lốc', 1188, '2026-05-01', 25000.00),
(15, 'Sữa chua uống Probi Việt quất', 'Lốc', 999, '2026-05-05', 25000.00),
(16, 'Sữa đặc Ông Thọ Đỏ', 'Lon', 290, '2027-01-01', 22000.00),
(17, 'Sữa đặc Ông Thọ Xanh', 'Lon', 394, '2027-02-01', 18000.00),
(18, 'Sữa đặc Ngôi Sao Phương Nam Xanh', 'Lon', 500, '2027-03-01', 16000.00),
(19, 'Sữa đậu nành GoldSoy', 'Hộp', 600, '2026-09-01', 15000.00),
(20, 'Sữa hạt Óc chó Vinamilk', 'Lốc', 200, '2026-08-15', 45000.00),
(21, 'Sữa hạt Hạnh nhân Vinamilk', 'Lốc', 180, '2026-08-10', 45000.00),
(22, 'Sữa tươi 100% Organic', 'Hộp', 100, '2026-10-30', 55000.00),
(23, 'Sữa bột Grow Plus đỏ', 'Hộp', 70, '2026-07-20', 320000.00),
(24, 'Sữa bột Grow Plus xanh', 'Hộp', 84, '2026-07-25', 290000.00),
(25, 'Sữa Kun túi Dâu', 'Túi', 2000, '2026-06-30', 5000.00),
(26, 'Sữa Kun túi Cam', 'Túi', 1800, '2026-06-25', 5000.00),
(27, 'Sữa trái cây Vfresh Cam', 'Chai', 500, '2026-11-01', 12000.00),
(28, 'Sữa trái cây Vfresh Táo', 'Chai', 450, '2026-11-05', 12000.00),
(29, 'Sữa bột Sure Prevent', 'Hộp', 19, '2026-09-10', 450000.00),
(30, 'Sữa bột CanxiPro', 'Hộp', 40, '2026-09-15', 380000.00),
(31, 'Sữa tươi tiệt trùng 1L Có đường', 'Hộp', 200, '2026-12-05', 35000.00),
(32, 'Sữa tươi tiệt trùng 1L Không đường', 'Hộp', 220, '2026-12-10', 35000.00),
(33, 'Sữa bột Dielac Grow Plus 2+', 'Hộp', 36, '2026-08-20', 310000.00),
(34, 'Sữa bột Dielac Pedia', 'Hộp', 30, '2026-08-25', 340000.00),
(35, 'Sữa tươi Flex không Lactose', 'Hộp', 150, '2026-11-15', 32000.00),
(36, 'Sữa bột Diecerna cho người tiểu đường', 'Hộp', 16, '2026-10-01', 520000.00),
(37, 'Sữa hạt Đậu đỏ Vinamilk', 'Lốc', 400, '2026-09-10', 12000.00),
(38, 'Sữa hạt Đậu nành nha đam', 'Lốc', 350, '2026-09-05', 13000.00),
(39, 'Sữa bột Optimum Comfort', 'Hộp', 20, '2026-07-01', 390000.00),
(40, 'Sữa bột Dielac Mama Gold', 'Hộp', 50, '2026-06-10', 220000.00),
(41, 'Sữa tươi ADM Có đường 110ml', 'Thùng', 3000, '2026-05-30', 4500.00),
(42, 'Sữa tươi ADM Socola 110ml', 'Thùng', 2800, '2026-05-25', 4500.00),
(43, 'Sữa chua uống Susu Dâu', 'Lốc', 4000, '2026-04-30', 4000.00),
(44, 'Sữa chua uống Susu Cam', 'Lốc', 3800, '2026-04-25', 4000.00),
(45, 'Sữa bột Ridielac Gạo sữa', 'Hộp', 90, '2026-09-20', 55000.00),
(46, 'Sữa bột Ridielac Gà rau củ', 'Hộp', 90, '2026-09-25', 58000.00),
(47, 'Sữa tươi tách béo Vinamilk', 'Hộp', 120, '2026-11-10', 33000.00),
(48, 'Sữa chua ăn Love Yogurt', 'Lốc', 500, '2026-04-05', 35000.00),
(49, 'Nước bù khoáng ICY', 'Chai', 949, '2027-01-10', 10000.00),
(50, 'Trà Atiso Vfresh', 'Chai', 800, '2027-01-05', 11000.00);

-- --------------------------------------------------------

--
-- Table structure for table `khachhang`
--

CREATE TABLE `khachhang` (
  `MaKhach` int(11) NOT NULL,
  `TenKhach` varchar(100) NOT NULL,
  `SoDienThoai` varchar(15) DEFAULT NULL,
  `DiaChi` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `khachhang`
--

INSERT INTO `khachhang` (`MaKhach`, `TenKhach`, `SoDienThoai`, `DiaChi`) VALUES
(1, 'Tạp hóa Cô Lan', '0901000001', '123 Nguyễn Trãi, Phường Bến Thành, Quận 1, TP.HCM'),
(2, 'Cửa hàng Minh Anh', '0901000002', '456 Lê Lợi, Phường Bến Nghé, Quận 1, TP.HCM'),
(3, 'Đại lý Nu', '0901000003', '789 Cách Mạng Tháng 8, Phường 10, Quận 3, TP.HCM'),
(4, 'Tiệm tạp hóa 79', '0901000004', '12 Nguyễn Thị Minh Khai, Phường Đa Kao, Quận 1, TP.HCM'),
(5, 'Siêu thị mini Win', '0901000005', '34 Lý Tự Trọng, Phường Bến Thành, Quận 1, TP.HCM'),
(6, 'Cửa hàng Tiện Lợi', '0901000006', '56 Nguyễn Huệ, Phường Bến Nghé, Quận 1, TP.HCM'),
(7, 'Tạp hóa Bác Ba', '0901000007', '78 Phạm Ngũ Lão, Phường Phạm Ngũ Lão, Quận 1, TP.HCM'),
(8, 'Đại lý Sữa Việt', '0901000008', '90 Võ Văn Tần, Phường Võ Thị Sáu, Quận 3, TP.HCM'),
(9, 'Cửa hàng Mẹ và Bé', '0901000009', '23 Điện Biên Phủ, Phường Đa Kao, Quận 1, TP.HCM'),
(10, 'Tạp hóa Hồng Đào', '0901000010', '45 Hai Bà Trưng, Phường Bến Nghé, Quận 1, TP.HCM'),
(11, 'Đại lý An Nhiên', '0901000011', '67 Lê Văn Sỹ, Phường 13, Quận 3, TP.HCM'),
(12, 'Cửa hàng bách hóa số 5', '0901000012', '89 Cách Mạng Tháng 8, Phường 11, Quận 3, TP.HCM'),
(13, 'Tiệm sữa Kim Liên', '0901000013', '101 Nguyễn Đình Chiểu, Phường 5, Quận 3, TP.HCM'),
(14, 'Tạp hóa Thanh Xuân', '0901000014', '112 Trần Hưng Đạo, Phường Cô Giang, Quận 1, TP.HCM'),
(15, 'Đại lý sữa Hùng Vương', '0901000015', '123 Bàu Cát, Phường 14, Quận Tân Bình, TP.HCM'),
(16, 'Cửa hàng sữa bột Linh Anh', '0901000016', '234 Cộng Hòa, Phường 4, Quận Tân Bình, TP.HCM'),
(17, 'Tạp hóa Chị Tư', '0901000017', '345 Lạc Long Quân, Phường 5, Quận 11, TP.HCM'),
(18, 'Đại lý bách hóa Toàn Cầu', '0901000018', '456 Âu Cơ, Phường 10, Quận Tân Bình, TP.HCM'),
(19, 'Siêu thị tiện ích Bee', '0901000019', '567 Nguyễn Văn Cừ, Phường 1, Quận 5, TP.HCM'),
(20, 'Cửa hàng sữa tươi 247', '0901000020', '678 Trần Phú, Phường 4, Quận 5, TP.HCM');

-- --------------------------------------------------------

--
-- Table structure for table `phieunhap`
--

CREATE TABLE `phieunhap` (
  `MaNhap` int(11) NOT NULL,
  `NgayNhap` datetime DEFAULT current_timestamp(),
  `NhaCungCap` varchar(200) DEFAULT NULL,
  `TrangThai` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `phieunhap`
--

INSERT INTO `phieunhap` (`MaNhap`, `NgayNhap`, `NhaCungCap`, `TrangThai`) VALUES
(1, '2026-03-30 00:00:00', 'Kho tổng Vinamilk', 'Đã hoàn thành'),
(2, '2026-03-30 00:00:00', 'Kho tổng Vinamilk', 'Đang chờ'),
(3, '2026-03-30 00:00:00', 'Kho tổng Vinamilk', 'Đã hoàn thành'),
(4, '2026-03-30 00:00:00', 'Kho tổng Vinamilk', 'Đã hoàn thành'),
(5, '2025-03-15 10:30:00', 'Kho tổng Vinamilk', 'Đang chờ'),
(6, '2025-03-20 14:15:00', 'Nhà máy sữa Bình Dương', 'Đang chờ'),
(7, '2025-03-10 09:00:00', 'Kho tổng Vinamilk', 'Đã hoàn thành'),
(8, '2025-03-05 11:20:00', 'Nhà máy sữa Nghệ An', 'Đã hoàn thành'),
(9, '2026-03-30 00:00:00', 'Kho tổng Vinamilk', 'Đã hoàn thành');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `chitietdon`
--
ALTER TABLE `chitietdon`
  ADD PRIMARY KEY (`MaDon`,`MaHang`),
  ADD KEY `MaHang` (`MaHang`);

--
-- Indexes for table `chitietphieunhap`
--
ALTER TABLE `chitietphieunhap`
  ADD PRIMARY KEY (`MaNhap`,`MaHang`),
  ADD KEY `MaHang` (`MaHang`);

--
-- Indexes for table `donhang`
--
ALTER TABLE `donhang`
  ADD PRIMARY KEY (`MaDon`),
  ADD KEY `MaKhach` (`MaKhach`);

--
-- Indexes for table `hanghoa`
--
ALTER TABLE `hanghoa`
  ADD PRIMARY KEY (`MaHang`);

--
-- Indexes for table `khachhang`
--
ALTER TABLE `khachhang`
  ADD PRIMARY KEY (`MaKhach`);

--
-- Indexes for table `phieunhap`
--
ALTER TABLE `phieunhap`
  ADD PRIMARY KEY (`MaNhap`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `donhang`
--
ALTER TABLE `donhang`
  MODIFY `MaDon` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24;

--
-- AUTO_INCREMENT for table `hanghoa`
--
ALTER TABLE `hanghoa`
  MODIFY `MaHang` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=51;

--
-- AUTO_INCREMENT for table `khachhang`
--
ALTER TABLE `khachhang`
  MODIFY `MaKhach` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=22;

--
-- AUTO_INCREMENT for table `phieunhap`
--
ALTER TABLE `phieunhap`
  MODIFY `MaNhap` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=10;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `chitietdon`
--
ALTER TABLE `chitietdon`
  ADD CONSTRAINT `chitietdon_ibfk_1` FOREIGN KEY (`MaDon`) REFERENCES `donhang` (`MaDon`),
  ADD CONSTRAINT `chitietdon_ibfk_2` FOREIGN KEY (`MaHang`) REFERENCES `hanghoa` (`MaHang`);

--
-- Constraints for table `chitietphieunhap`
--
ALTER TABLE `chitietphieunhap`
  ADD CONSTRAINT `chitietphieunhap_ibfk_1` FOREIGN KEY (`MaNhap`) REFERENCES `phieunhap` (`MaNhap`),
  ADD CONSTRAINT `chitietphieunhap_ibfk_2` FOREIGN KEY (`MaHang`) REFERENCES `hanghoa` (`MaHang`);

--
-- Constraints for table `donhang`
--
ALTER TABLE `donhang`
  ADD CONSTRAINT `donhang_ibfk_1` FOREIGN KEY (`MaKhach`) REFERENCES `khachhang` (`MaKhach`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
