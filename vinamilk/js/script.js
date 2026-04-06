// ==========================================
// 1. BIẾN TOÀN CỤC (Chờ dữ liệu từ API)
// ==========================================
window.ordersDb = []; 
window.productsList = []; // Chứa danh sách sản phẩm lấy từ DB để đổ vào Dropdown

// ==========================================
// 2. CÁC HÀM GIAO TIẾP VỚI SERVER (API)
// ==========================================

// Tải dữ liệu từ Server khi trang web vừa mở
window.loadServerData = async function() {
    try {
        // Lấy danh sách phiếu nhập
        const resOrders = await fetch('api_nhaphang.php?action=get_orders');
        window.ordersDb = await resOrders.json();
        
        // Lấy danh sách hàng hóa cho Dropdown
        const resProducts = await fetch('api_nhaphang.php?action=get_products');
        window.productsList = await resProducts.json();

        // Render lại bảng với dữ liệu thật theo bộ lọc hiện tại
        const currentFilter = document.getElementById("statusFilter") ? document.getElementById("statusFilter").value : 'all';
        window.renderHistoryTable(currentFilter);
    } catch (error) {
        console.error("Lỗi tải dữ liệu:", error);
    }
}

// ==========================================
// 3. CÁC HÀM XỬ LÝ GIAO DIỆN TOÀN CỤC
// ==========================================

// Hàm Render Bảng Lịch Sử
window.renderHistoryTable = function(filterStatus) { 
    const historyTableBody = document.getElementById("historyTableBody");
    if (!historyTableBody) return;
    historyTableBody.innerHTML = "";
    
    // Nếu mảng rỗng (chưa có dữ liệu từ DB) hoặc lỗi
    if (!window.ordersDb || window.ordersDb.length === 0 || window.ordersDb.error) {
        historyTableBody.innerHTML = `<tr><td colspan="6" class="text-center">Chưa có dữ liệu phiếu nhập.</td></tr>`;
        return;
    }

    // Lọc dữ liệu theo trạng thái
    const filteredOrders = window.ordersDb.filter(order => {
        if (filterStatus === "all") return true;
        // Giả sử DB lưu N'Đang chờ' và N'Đã hoàn thành'
        if (filterStatus === "pending") return order.TrangThai === "Đang chờ" || order.TrangThai === "Nợ" || !order.TrangThai; 
        if (filterStatus === "completed") return order.TrangThai === "Đã hoàn thành";
        return true;
    });

    filteredOrders.forEach(order => {
        const isPending = order.TrangThai === "Đang chờ" || order.TrangThai === "Nợ" || !order.TrangThai;
        const statusBadge = isPending 
            ? `<span class="badge-pending" style="color:#ffc107; font-weight:bold;">⏳ Chờ xác nhận</span>` 
            : `<span class="badge-completed" style="color:#28a745; font-weight:bold;">✅ Đã hoàn thành</span>`;
        
        const confirmBtn = isPending 
            ? `<button class="btn-action-confirm" onclick="confirmOrder('${order.MaNhap}')" style="background:#28a745; color:white; border:none; padding:5px 10px; border-radius:5px; cursor:pointer;"><i class="fas fa-check"></i> Xác nhận</button>` 
            : "";

        // Format ngày tháng (YYYY-MM-DD sang DD/MM/YYYY)
        const dateObj = new Date(order.NgayNhap);
        const formattedDate = !isNaN(dateObj) ? dateObj.toLocaleDateString('vi-VN') : order.NgayNhap;

        const rowHTML = `
            <tr>
                <td><strong>PN-${order.MaNhap}</strong></td>
                <td>${formattedDate}</td>
                <td>${order.NhaCungCap}</td>
                <td><strong>${Number(order.TongSoLuong || 0).toLocaleString('vi-VN')}</strong></td>
                <td>${statusBadge}</td>
                <td class="text-center">
                <button class="btn-action-view" onclick="viewOrderDetails('${order.MaNhap}')" style="background:#17a2b8; color:white; border:none; padding:5px 10px; border-radius:5px; cursor:pointer; margin-right:5px;"><i class="fas fa-eye"></i> Xem</button>                    ${confirmBtn}
                </td>
            </tr>
        `;
        historyTableBody.insertAdjacentHTML('beforeend', rowHTML);
    });
};

// Hàm kích hoạt Bộ Lọc
window.filterOrders = function(status) {
    window.renderHistoryTable(status);
};

// Hàm Xác Nhận (Gọi API Update DB & Cộng kho qua Trigger)
window.confirmOrder = async function(orderId) {
    if(confirm(`Admin có chắc chắn XÁC NHẬN phiếu PN-${orderId} không?\nSố lượng sẽ được cộng trực tiếp vào Tồn Kho DB.`)) {
        try {
            // Gửi ID lên server để chuyển trạng thái thành 'Đã hoàn thành'
            const response = await fetch('api_nhaphang.php?action=confirm_order', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `maNhap=${orderId}`
            });
            
            const result = await response.json();
            
            if(result.success) {
                alert(`✅ XÁC NHẬN THÀNH CÔNG!\nPhiếu PN-${orderId} đã được xử lý xong và hàng đã vào kho.`);
                window.loadServerData(); // Tải lại toàn bộ dữ liệu mới nhất từ DB
            } else {
                alert("Lỗi từ server: " + (result.error || "Không xác định"));
            }
        } catch (error) {
            console.error("Lỗi:", error);
            alert("Không thể kết nối đến máy chủ.");
        }
    }
};

// Hàm Gọi API Lấy và Xem Chi Tiết Phiếu Nhập
window.viewOrderDetails = async function(orderId) {
    try {
        const response = await fetch(`api_nhaphang.php?action=get_order_details&id=${orderId}`);
        const details = await response.json();

        if (details.error) {
            alert("Lỗi: " + details.error);
            return;
        }

        // Đổ dữ liệu vào Modal View
        document.getElementById("viewOrderId").innerText = `PN-${orderId}`;
        const tbody = document.getElementById("viewOrderDetailsBody");
        tbody.innerHTML = "";

        if (details.length === 0) {
            tbody.innerHTML = `<tr><td colspan="4" class="text-center">Phiếu này không có mặt hàng nào.</td></tr>`;
        } else {
            details.forEach(item => {
                tbody.insertAdjacentHTML('beforeend', `
                    <tr>
                        <td><strong>${item.MaHang}</strong></td>
                        <td>${item.TenHang}</td>
                        <td>${item.DonViTinh}</td>
                        <td style="font-weight: bold; color: #d9534f;">${item.SoLuong}</td>
                    </tr>
                `);
            });
        }

        // Mở Modal
        document.getElementById("viewOrderModal").style.display = "flex";

    } catch (error) {
        console.error("Lỗi:", error);
        alert("Không thể tải chi tiết phiếu nhập.");
    }
};

// Sự kiện đóng Modal Xem chi tiết
document.addEventListener("DOMContentLoaded", function () {
    const closeViewModal = () => document.getElementById("viewOrderModal").style.display = "none";
    document.getElementById("closeViewModalBtn")?.addEventListener("click", closeViewModal);
    document.getElementById("btnCancelViewModal")?.addEventListener("click", closeViewModal);
});

// Quản lý Modal
window.openModal = function() {
    document.getElementById("orderModal").style.display = "flex";
}
window.closeModal = function() {
    document.getElementById("orderModal").style.display = "none";
}

// ==========================================
// 4. KHỞI TẠO & SỰ KIỆN KHI TRANG LOAD
// ==========================================
document.addEventListener("DOMContentLoaded", function () {
    const statusFilter = document.getElementById("statusFilter");
    const productTableBody = document.getElementById("productTableBody");
    
    if (!productTableBody) return; 

    // Bắt buộc mặc định lọc phiếu Pending
    statusFilter.value = "pending"; 

    // KÍCH HOẠT TẢI DỮ LIỆU TỪ SERVER
    window.loadServerData(); 

    document.getElementById("orderDate").valueAsDate = new Date();

    // Sinh các dòng sản phẩm động từ DB
    function getProductRowHTML() {
        let optionsHTML = '<option value="">-- Chọn sản phẩm --</option>';
        if (window.productsList && window.productsList.length > 0) {
            window.productsList.forEach(prod => {
                optionsHTML += `<option value="${prod.MaHang}">${prod.TenHang} (${prod.DonViTinh})</option>`;
            });
        }

        return `
            <tr>
                <td>
                    <select class="item-id" style="width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 5px;">
                        ${optionsHTML}
                    </select>
                </td>
                <td><input type="number" min="1" value="1" class="item-qty" style="width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 5px;"></td>
                <td><input type="text" class="item-note" placeholder="Ghi chú..." style="width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 5px;"></td>
                <td class="text-center col-action">
                    <button onclick="this.closest('tr').remove()" style="color: red; background: none; border: none; cursor: pointer;"><i class="fas fa-trash"></i></button>
                </td>
            </tr>
        `;
    }

    document.getElementById("btnAddRow").addEventListener("click", function() {
        document.getElementById("productTableBody").insertAdjacentHTML('beforeend', getProductRowHTML());
    });

    // Mở modal thêm mới
    document.getElementById("btnCreateNew").addEventListener("click", function() {
        document.getElementById("formTitle").innerText = "Lập Phiếu Nhập Dự Trù";
        document.getElementById("orderId").value = "Hệ thống tự cấp ID"; // DB có IDENTITY tự tăng
        document.getElementById("orderDate").disabled = false;
        document.getElementById("supplier").disabled = false;
        document.getElementById("btnAddRow").style.display = "inline-block";
        document.getElementById("formActionButtons").style.display = "flex";
        document.getElementById("colAction").style.display = "table-cell";
        
        document.getElementById("productTableBody").innerHTML = "";
        document.getElementById("productTableBody").insertAdjacentHTML('beforeend', getProductRowHTML()); 
        
        window.openModal();
    });

    // Tắt modal
    document.getElementById("closeModalBtn").addEventListener("click", window.closeModal);
    document.getElementById("btnCancelModal").addEventListener("click", window.closeModal);
    window.addEventListener("click", function(event) {
        if (event.target === document.getElementById("orderModal")) {
            window.closeModal();
        }
    });

    // ==========================================
    // XỬ LÝ LƯU PHIẾU LÊN SERVER (POST API)
    // ==========================================
    document.getElementById("btnSaveDraft").addEventListener("click", async function() {
        const rows = document.getElementById("productTableBody").querySelectorAll("tr");
        if(rows.length === 0) {
            alert("Vui lòng thêm ít nhất 1 mặt hàng!"); 
            return;
        }

        let isValid = true;
        const itemsToSave = [];

        // Kiểm tra hợp lệ từng dòng
        rows.forEach(row => {
            const productId = row.querySelector(".item-id").value;
            const qty = parseInt(row.querySelector(".item-qty").value);
            
            if (!productId || isNaN(qty) || qty <= 0) {
                isValid = false;
            } else {
                itemsToSave.push({ id: productId, qty: qty });
            }
        });

        if (!isValid) {
            alert("Lỗi: Vui lòng chọn sản phẩm và nhập số lượng lớn hơn 0 cho tất cả mặt hàng!");
            return; 
        }

        const payload = {
            supplier: document.getElementById("supplier").value,
            date: document.getElementById("orderDate").value,
            items: itemsToSave
        };

        // Vô hiệu hóa nút lưu để tránh click 2 lần
        const btnSave = document.getElementById("btnSaveDraft");
        btnSave.disabled = true;
        btnSave.innerText = "Đang xử lý...";

        try {
            // Gửi dữ liệu lên API bằng POST
            const response = await fetch('api_nhaphang.php?action=create_order', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            
            const result = await response.json();
            
            if(result.success) {
                alert("Đã tạo Phiếu Dự Trù thành công vào Database!\nTrạng thái: Đang chờ Admin xác nhận.");
                window.closeModal(); 
                statusFilter.value = "pending";
                window.loadServerData(); // Load lại data từ Server
            } else {
                alert("Lỗi từ server: " + (result.error || "Không xác định"));
            }
        } catch (error) {
            console.error(error);
            alert("Không thể kết nối tới server. Vui lòng kiểm tra lại api_nhaphang.php");
        } finally {
            // Khôi phục trạng thái nút bấm
            btnSave.disabled = false;
            btnSave.innerHTML = `<i class="fas fa-save"></i> Gửi phiếu dự trù`;
        }
    });
});