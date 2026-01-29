# Cách đổi
*1KB = 1024 byte*

*⚪ = frame*

*🔵 = page*

*□ : segment*
 
*○: offset*
 
**Địa chỉ vật lý -> logic**

VD: Đổi ⬜ sang địa chỉ logic, với kích thước mỗi trang là 1KB
⬜/1024 = ⚪(frame) => tìm được page (🔵)
Địa chỉ logic = page * 1024 + (⬜ % 1024)

**Địa chỉ logic -> vật lý**

VD: Đổi ⬜ sang địa chỉ vật lý, với kích thước mỗi trang là 1KB
⬜/1024 = 🔵(page) => tìm được frame (⚪)
Địa chỉ vật lý = frame * 1024 + (⬜ % 1024)

**Nếu ⬜ là số thập phân**

VD: tính địa chỉ vật lý tương ứng với địa chỉ logic (□,○)
- Nhìn bảng phân đoạn, từ □ tìm ra base tương ứng
==> Địa chỉ vật lý = base + offset
**Điều kiện**
  offset < length

**Note**

*Địa chỉ vật lý -> logic*
⬜ % 1024 tính nhanh bằng cách: ⬜ - ( ⚪ * 1024 )

*Địa chỉ logic -> vật lý*
⬜ % 1024 tính nhanh bằng cách: ⬜ - ( 🔵 * 1024 )
