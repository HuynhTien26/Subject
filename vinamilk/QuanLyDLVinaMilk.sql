-- Tạo và sử dụng cơ sở dữ liệu
CREATE DATABASE IF NOT EXISTS QuanLyDLVinamilk;
USE QuanLyDLVinamilk;

-- =============================================
-- 1. TẠO CÁC BẢNG DỮ LIỆU
-- =============================================

DROP TABLE IF EXISTS CHITIETPHIEUNHAP;
DROP TABLE IF EXISTS CHITIETDON;
DROP TABLE IF EXISTS PHIEUNHAP;
DROP TABLE IF EXISTS DONHANG;
DROP TABLE IF EXISTS HANGHOA;
DROP TABLE IF EXISTS KHACHHANG;

CREATE TABLE KHACHHANG (
    MaKhach INT PRIMARY KEY AUTO_INCREMENT,
    TenKhach VARCHAR(100) NOT NULL,
    SoDienThoai VARCHAR(15),
    CongNo DECIMAL(18,2) DEFAULT 0
);

CREATE TABLE HANGHOA (
    MaHang INT PRIMARY KEY AUTO_INCREMENT,
    TenHang VARCHAR(100) NOT NULL,
    DonViTinh VARCHAR(20),
    SoLuongTonKho INT DEFAULT 0,
    HanSuDung DATE,
    DonGia DECIMAL(18,2)
);

CREATE TABLE DONHANG (
    MaDon INT PRIMARY KEY AUTO_INCREMENT,
    NgayBan DATETIME DEFAULT CURRENT_TIMESTAMP,
    MaKhach INT,
    TongTien DECIMAL(18,2) DEFAULT 0,
    TrangThai VARCHAR(50),
    FOREIGN KEY (MaKhach) REFERENCES KHACHHANG(MaKhach)
);

CREATE TABLE PHIEUNHAP (
    MaNhap INT PRIMARY KEY AUTO_INCREMENT,
    NgayNhap DATETIME DEFAULT CURRENT_TIMESTAMP,
    NhaCungCap VARCHAR(200),
    TrangThai VARCHAR(50)
);

CREATE TABLE CHITIETDON (
    MaDon INT,
    MaHang INT,
    SoLuong INT,
    DonGia DECIMAL(18,2),
    PRIMARY KEY (MaDon, MaHang),
    FOREIGN KEY (MaDon) REFERENCES DONHANG(MaDon),
    FOREIGN KEY (MaHang) REFERENCES HANGHOA(MaHang)
);

CREATE TABLE CHITIETPHIEUNHAP (
    MaNhap INT,
    MaHang INT,
    SoLuong INT,
    PRIMARY KEY (MaNhap, MaHang),
    FOREIGN KEY (MaNhap) REFERENCES PHIEUNHAP(MaNhap),
    FOREIGN KEY (MaHang) REFERENCES HANGHOA(MaHang)
);

-- =============================================
-- 2. CÀI ĐẶT TRIGGER
-- =============================================

DELIMITER //

-- Trigger Bán hàng: trừ kho, tính tiền, cập nhật công nợ
CREATE TRIGGER TRG_XuLyBanHang
AFTER INSERT ON CHITIETDON
FOR EACH ROW
BEGIN
    -- 1. Cập nhật đơn giá từ bảng HANGHOA
    UPDATE CHITIETDON
    SET DonGia = (SELECT DonGia FROM HANGHOA WHERE MaHang = NEW.MaHang)
    WHERE MaDon = NEW.MaDon AND MaHang = NEW.MaHang;

    -- 2. Trừ tồn kho
    UPDATE HANGHOA
    SET SoLuongTonKho = SoLuongTonKho - NEW.SoLuong
    WHERE MaHang = NEW.MaHang;

    -- 3. Cập nhật tổng tiền đơn hàng
    UPDATE DONHANG
    SET TongTien = (
        SELECT SUM(SoLuong * DonGia)
        FROM CHITIETDON
        WHERE MaDon = NEW.MaDon
    )
    WHERE MaDon = NEW.MaDon;

    -- 4. Nếu đơn hàng ở trạng thái 'Nợ', cộng công nợ khách hàng
    UPDATE KHACHHANG
    SET CongNo = CongNo + (NEW.SoLuong * (SELECT DonGia FROM HANGHOA WHERE MaHang = NEW.MaHang))
    WHERE MaKhach = (SELECT MaKhach FROM DONHANG WHERE MaDon = NEW.MaDon)
      AND (SELECT TrangThai FROM DONHANG WHERE MaDon = NEW.MaDon) = 'Nợ';
END;
//

DELIMITER ;
-- Xóa trigger nếu tồn tại
DROP TRIGGER IF EXISTS TRG_XuLyBanHang;

-- Chèn đơn hàng
INSERT INTO DONHANG (NgayBan, MaKhach, TongTien, TrangThai) VALUES
('2025-03-28 09:30:00', 1, 0, 'Đang xử lý'),
('2025-03-27 14:15:00', 2, 0, 'Đang giao'),
('2025-03-26 11:00:00', 3, 0, 'Đã hoàn thành'),
('2025-03-29 08:45:00', 4, 0, 'Đang xử lý');

-- Lấy ID của các đơn vừa chèn
SET @order1 = LAST_INSERT_ID();
SET @order2 = @order1 + 1;
SET @order3 = @order2 + 1;
SET @order4 = @order3 + 1;

-- Chèn chi tiết (sử dụng SELECT để lấy giá)
INSERT INTO CHITIETDON (MaDon, MaHang, SoLuong, DonGia)
SELECT * FROM (
    SELECT @order1, 1, 10, (SELECT DonGia FROM HANGHOA WHERE MaHang = 1)
    UNION ALL
    SELECT @order1, 2, 5, (SELECT DonGia FROM HANGHOA WHERE MaHang = 2)
    UNION ALL
    SELECT @order2, 3, 8, (SELECT DonGia FROM HANGHOA WHERE MaHang = 3)
    UNION ALL
    SELECT @order2, 4, 12, (SELECT DonGia FROM HANGHOA WHERE MaHang = 4)
    UNION ALL
    SELECT @order3, 5, 6, (SELECT DonGia FROM HANGHOA WHERE MaHang = 5)
    UNION ALL
    SELECT @order4, 6, 2, (SELECT DonGia FROM HANGHOA WHERE MaHang = 6)
    UNION ALL
    SELECT @order4, 7, 3, (SELECT DonGia FROM HANGHOA WHERE MaHang = 7)
) AS tmp;

-- Cập nhật tồn kho (dùng CASE)
UPDATE HANGHOA
SET SoLuongTonKho = SoLuongTonKho - 
    CASE MaHang
        WHEN 1 THEN 10
        WHEN 2 THEN 5
        WHEN 3 THEN 8
        WHEN 4 THEN 12
        WHEN 5 THEN 6
        WHEN 6 THEN 2
        WHEN 7 THEN 3
        ELSE 0
    END
WHERE MaHang IN (1,2,3,4,5,6,7);

-- Cập nhật tổng tiền đơn hàng (dùng JOIN)
UPDATE DONHANG d
JOIN (
    SELECT MaDon, SUM(SoLuong * DonGia) AS Tong
    FROM CHITIETDON
    GROUP BY MaDon
) t ON d.MaDon = t.MaDon
SET d.TongTien = t.Tong;

-- Chèn đơn hàng
INSERT INTO DONHANG (NgayBan, MaKhach, TongTien, TrangThai) VALUES
('2025-03-28 09:30:00', 1, 0, 'Đang xử lý'),
('2025-03-27 14:15:00', 2, 0, 'Đang giao'),
('2025-03-26 11:00:00', 3, 0, 'Đã hoàn thành'),
('2025-03-29 08:45:00', 4, 0, 'Đang xử lý');

-- Lấy ID của các đơn vừa chèn (nếu cần dùng trong chi tiết)
SET @order1 = LAST_INSERT_ID();
SET @order2 = @order1 + 1;
SET @order3 = @order2 + 1;
SET @order4 = @order3 + 1;

-- Chèn chi tiết (dùng VALUES nhiều dòng)
INSERT INTO CHITIETDON (MaDon, MaHang, SoLuong, DonGia) VALUES
(@order1, 1, 10, (SELECT DonGia FROM HANGHOA WHERE MaHang = 1)),
(@order1, 2, 5, (SELECT DonGia FROM HANGHOA WHERE MaHang = 2)),
(@order2, 3, 8, (SELECT DonGia FROM HANGHOA WHERE MaHang = 3)),
(@order2, 4, 12, (SELECT DonGia FROM HANGHOA WHERE MaHang = 4)),
(@order3, 5, 6, (SELECT DonGia FROM HANGHOA WHERE MaHang = 5)),
(@order4, 6, 2, (SELECT DonGia FROM HANGHOA WHERE MaHang = 6)),
(@order4, 7, 3, (SELECT DonGia FROM HANGHOA WHERE MaHang = 7));

-- Cập nhật tồn kho (cũng có thể gộp)
UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho - 
    CASE MaHang
        WHEN 1 THEN 10
        WHEN 2 THEN 5
        WHEN 3 THEN 8
        WHEN 4 THEN 12
        WHEN 5 THEN 6
        WHEN 6 THEN 2
        WHEN 7 THEN 3
        ELSE 0
    END
WHERE MaHang IN (1,2,3,4,5,6,7);

-- Cập nhật tổng tiền (dùng JOIN)
UPDATE DONHANG d
JOIN (
    SELECT MaDon, SUM(SoLuong * DonGia) AS Tong
    FROM CHITIETDON
    GROUP BY MaDon
) t ON d.MaDon = t.MaDon
SET d.TongTien = t.Tong;

// Dữ liệu mẫu cho phiếu nhập
INSERT INTO PHIEUNHAP (NgayNhap, NhaCungCap, TrangThai) VALUES
('2025-03-15 10:30:00', 'Kho tổng Vinamilk', 'Đang chờ');
SET @id1 = LAST_INSERT_ID();

INSERT INTO PHIEUNHAP (NgayNhap, NhaCungCap, TrangThai) VALUES
('2025-03-20 14:15:00', 'Nhà máy sữa Bình Dương', 'Đang chờ');
SET @id2 = LAST_INSERT_ID();

INSERT INTO PHIEUNHAP (NgayNhap, NhaCungCap, TrangThai) VALUES
('2025-03-10 09:00:00', 'Kho tổng Vinamilk', 'Đã hoàn thành');
SET @id3 = LAST_INSERT_ID();

INSERT INTO PHIEUNHAP (NgayNhap, NhaCungCap, TrangThai) VALUES
('2025-03-05 11:20:00', 'Nhà máy sữa Nghệ An', 'Đã hoàn thành');
SET @id4 = LAST_INSERT_ID();

-- =============================================
-- THÊM CHI TIẾT CHO TỪNG PHIẾU
-- =============================================

INSERT INTO CHITIETPHIEUNHAP (MaNhap, MaHang, SoLuong) VALUES
(@id1, 1, 10),
(@id1, 2, 5),
(@id1, 3, 8);

INSERT INTO CHITIETPHIEUNHAP (MaNhap, MaHang, SoLuong) VALUES
(@id2, 4, 12),
(@id2, 5, 6);

INSERT INTO CHITIETPHIEUNHAP (MaNhap, MaHang, SoLuong) VALUES
(@id3, 10, 3),
(@id3, 11, 20);

INSERT INTO CHITIETPHIEUNHAP (MaNhap, MaHang, SoLuong) VALUES
(@id4, 15, 7),
(@id4, 16, 4);

-- Cộng kho cho phiếu 3
UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho + 3 WHERE MaHang = 10;
UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho + 20 WHERE MaHang = 11;

-- Cộng kho cho phiếu 4
UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho + 7 WHERE MaHang = 15;
UPDATE HANGHOA SET SoLuongTonKho = SoLuongTonKho + 4 WHERE MaHang = 16;

-- =============================================
-- 3. DỮ LIỆU MẪU (20 KHÁCH HÀNG)
-- =============================================

INSERT INTO KHACHHANG (TenKhach, SoDienThoai) VALUES 
('Tạp hóa Cô Lan', '0901000001'),
('Cửa hàng Minh Anh', '0901000002'),
('Đại lý Nu', '0901000003'),
('Tiệm tạp hóa 79', '0901000004'),
('Siêu thị mini Win', '0901000005'),
('Cửa hàng Tiện Lợi', '0901000006'),
('Tạp hóa Bác Ba', '0901000007'),
('Đại lý Sữa Việt', '0901000008'),
('Cửa hàng Mẹ và Bé', '0901000009'),
('Tạp hóa Hồng Đào', '0901000010'),
('Đại lý An Nhiên', '0901000011'),
('Cửa hàng bách hóa số 5', '0901000012'),
('Tiệm sữa Kim Liên', '0901000013'),
('Tạp hóa Thanh Xuân', '0901000014'),
('Đại lý sữa Hùng Vương', '0901000015'),
('Cửa hàng sữa bột Linh Anh', '0901000016'),
('Tạp hóa Chị Tư', '0901000017'),
('Đại lý bách hóa Toàn Cầu', '0901000018'),
('Siêu thị tiện ích Bee', '0901000019'),
('Cửa hàng sữa tươi 247', '0901000020');

-- =============================================
-- 4. DỮ LIỆU MẪU (50 SẢN PHẨM SỮA)
-- =============================================

INSERT INTO HANGHOA (TenHang, DonViTinh, SoLuongTonKho, HanSuDung, DonGia) VALUES 
('Sữa tươi Vinamilk Có đường 180ml', 'Thùng', 500, '2026-12-01', 320000),
('Sữa tươi Vinamilk Ít đường 180ml', 'Thùng', 450, '2026-12-15', 320000),
('Sữa tươi Vinamilk Không đường 180ml', 'Thùng', 300, '2026-11-20', 320000),
('Sữa tươi Vinamilk Dâu 180ml', 'Thùng', 200, '2026-10-10', 330000),
('Sữa tươi Vinamilk Socola 180ml', 'Thùng', 250, '2026-10-05', 330000),
('Sữa bột Dielac Alpha Gold 1', 'Hộp', 50, '2026-05-20', 255000),
('Sữa bột Dielac Alpha Gold 2', 'Hộp', 45, '2026-06-15', 265000),
('Sữa bột Dielac Alpha Gold 3', 'Hộp', 40, '2026-07-10', 275000),
('Sữa bột Dielac Alpha Gold 4', 'Hộp', 35, '2026-08-05', 285000),
('Sữa bột Optimum Gold 1', 'Hộp', 30, '2026-05-12', 350000),
('Sữa chua Vinamilk Có đường', 'Lốc', 1000, '2026-04-15', 28000),
('Sữa chua Vinamilk Ít đường', 'Lốc', 800, '2026-04-20', 28000),
('Sữa chua Vinamilk Nha đam', 'Lốc', 600, '2026-04-10', 32000),
('Sữa chua uống Probi Dâu', 'Lốc', 1200, '2026-05-01', 25000),
('Sữa chua uống Probi Việt quất', 'Lốc', 1000, '2026-05-05', 25000),
('Sữa đặc Ông Thọ Đỏ', 'Lon', 300, '2027-01-01', 22000),
('Sữa đặc Ông Thọ Xanh', 'Lon', 400, '2027-02-01', 18000),
('Sữa đặc Ngôi Sao Phương Nam Xanh', 'Lon', 500, '2027-03-01', 16000),
('Sữa đậu nành GoldSoy', 'Hộp', 600, '2026-09-01', 15000),
('Sữa hạt Óc chó Vinamilk', 'Lốc', 200, '2026-08-15', 45000),
('Sữa hạt Hạnh nhân Vinamilk', 'Lốc', 180, '2026-08-10', 45000),
('Sữa tươi 100% Organic', 'Hộp', 100, '2026-10-30', 55000),
('Sữa bột Grow Plus đỏ', 'Hộp', 80, '2026-07-20', 320000),
('Sữa bột Grow Plus xanh', 'Hộp', 90, '2026-07-25', 290000),
('Sữa Kun túi Dâu', 'Túi', 2000, '2026-06-30', 5000),
('Sữa Kun túi Cam', 'Túi', 1800, '2026-06-25', 5000),
('Sữa trái cây Vfresh Cam', 'Chai', 500, '2026-11-01', 12000),
('Sữa trái cây Vfresh Táo', 'Chai', 450, '2026-11-05', 12000),
('Sữa bột Sure Prevent', 'Hộp', 20, '2026-09-10', 450000),
('Sữa bột CanxiPro', 'Hộp', 25, '2026-09-15', 380000),
('Sữa tươi tiệt trùng 1L Có đường', 'Hộp', 200, '2026-12-05', 35000),
('Sữa tươi tiệt trùng 1L Không đường', 'Hộp', 220, '2026-12-10', 35000),
('Sữa bột Dielac Grow Plus 2+', 'Hộp', 40, '2026-08-20', 310000),
('Sữa bột Dielac Pedia', 'Hộp', 30, '2026-08-25', 340000),
('Sữa tươi Flex không Lactose', 'Hộp', 150, '2026-11-15', 32000),
('Sữa bột Diecerna cho người tiểu đường', 'Hộp', 15, '2026-10-01', 520000),
('Sữa hạt Đậu đỏ Vinamilk', 'Lốc', 400, '2026-09-10', 12000),
('Sữa hạt Đậu nành nha đam', 'Lốc', 350, '2026-09-05', 13000),
('Sữa bột Optimum Comfort', 'Hộp', 20, '2026-07-01', 390000),
('Sữa bột Dielac Mama Gold', 'Hộp', 60, '2026-06-10', 220000),
('Sữa tươi ADM Có đường 110ml', 'Thùng', 3000, '2026-05-30', 4500),
('Sữa tươi ADM Socola 110ml', 'Thùng', 2800, '2026-05-25', 4500),
('Sữa chua uống Susu Dâu', 'Lốc', 4000, '2026-04-30', 4000),
('Sữa chua uống Susu Cam', 'Lốc', 3800, '2026-04-25', 4000),
('Sữa bột Ridielac Gạo sữa', 'Hộp', 100, '2026-09-20', 55000),
('Sữa bột Ridielac Gà rau củ', 'Hộp', 90, '2026-09-25', 58000),
('Sữa tươi tách béo Vinamilk', 'Hộp', 120, '2026-11-10', 33000),
('Sữa chua ăn Love Yogurt', 'Lốc', 500, '2026-04-05', 35000),
('Nước bù khoáng ICY', 'Chai', 1000, '2027-01-10', 10000),
('Trà Atiso Vfresh', 'Chai', 800, '2027-01-05', 11000);