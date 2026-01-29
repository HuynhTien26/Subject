# Nội dung
**Available**

Số lượng các thể hiện còn tự do của tài nguyên R(A)

**Max**

Nhu cầu tối đa của tiến trình P đối tài nguyên R (A)

**Allocation**

Số tài nguyên R thực sự cấp phát cho tiến trình P

**Need**

= Max - Allocation

**Cách tạo bảng và mẹo kiểm tra**

*Mẹo*

- Khi đề không cho số thể hiện của R (hoặc A) thì cộng cột bảng Allocation và Available lại
- Sau khi tạo được bảng Work thì kiểm tra xem tiến trình cuối cùng được làm xong có giống với số thể hiện ban đầu của đề cho/ bản thân cộng từ bảng Allo và Avai

*Tạo bảng Work*
B1. Cột đầu của Work là dòng của Available
B2. Dùng cột đầu vừa ghi vào bảng Work sau đó đem đi so sánh với từng P trong cột Available

*Nếu tài nguyên tự do >= tài nguyên cần*

Thì thu hồi bộ nhớ rồi + Allocation : tức là lấy dòng mình đang dùng so sánh ở bảng Work cộng với Allo tương ứng với P đang so sánh rồi ghi vào bảng Work
B3. Sau khi xong bảng Work thì kiểm tra xem đúng với số thể hiện ban đầu không, nếu đúng =>hệ thống an toàn
