// 1. Khai báo biến toàn cục
let inventoryDb = []; // Dữ liệu kho hàng từ API
let auditHistory = []; // Lịch sử kiểm kê
const CURRENT_DATE = new Date();

// ==========================================
// 2. RENDER GIAO DIỆN
// ==========================================

// Hàm A: Render Bảng Lô hàng cận date (Dùng chung data từ API)
function renderBatchTable() {
    const tbody = document.getElementById("batchTableBody");
    if (!tbody) return;
    tbody.innerHTML = "";

    let hasWarningItem = false;

    inventoryDb.forEach(item => {
        // Giả sử API trả về expiryDate, ta tính toán cận date
        const expDate = new Date(item.expiryDate);
        const diffTime = expDate - CURRENT_DATE;
        const diffDays = Math.ceil(diffTime / (1000 * 60 * 60 * 24));

        if (diffDays <= 30 && diffDays > 0) {
            hasWarningItem = true;
            const rowHTML = `
                <tr class="row-danger">
                    <td><strong>${item.id}</strong></td>
                    <td>${item.name}</td>
                    <td>${item.expiryDate}</td>
                    <td>${item.stock}</td>
                    <td><span class="badge-danger">Cận Date (${diffDays} ngày)</span></td>
                </tr>
            `;
            tbody.insertAdjacentHTML('beforeend', rowHTML);
        }
    });

    if (!hasWarningItem) {
        tbody.innerHTML = `<tr><td colspan="5" style="text-align: center; padding: 30px; color: #28a745;">Tuyệt vời! Không có hàng cận date.</td></tr>`;
    }
}

// Hàm B: Render Bảng Tồn Kho Tổng
function renderInventoryTable() {
    const tbody = document.getElementById("inventoryTableBody");
    if (!tbody) return;
    tbody.innerHTML = "";

    inventoryDb.forEach(prod => {
        const rowHTML = `
            <tr>
                <td><strong>${prod.id}</strong></td>
                <td>${prod.name}</td>
                <td>${prod.unit}</td>
                <td style="font-size: 16px; font-weight: bold; color: #0056b3;">${prod.stock}</td>
                <td>${new Intl.NumberFormat('vi-VN').format(prod.price)} đ</td>
            </tr>
        `;
        tbody.insertAdjacentHTML('beforeend', rowHTML);
    });
}

// ==========================================
// 3. LOGIC KIỂM KÊ (Sửa productsDb -> inventoryDb)
// ==========================================

function openAuditModal() {
    document.getElementById("auditId").value = "PKK-" + Math.floor(Math.random() * 10000);
    document.getElementById("auditDate").valueAsDate = new Date();
    
    const tbody = document.getElementById("auditTableItems");
    tbody.innerHTML = "";

    inventoryDb.forEach((prod, index) => {
        const rowHTML = `
            <tr data-index="${index}">
                <td><strong>${prod.id}</strong> - ${prod.name}</td>
                <td class="text-center"><span class="sys-qty">${prod.stock}</span></td>
                <td><input type="number" class="input-audit actual-qty" value="${prod.stock}" min="0"></td>
                <td class="text-center"><span class="diff-display diff-zero">0</span></td>
                <td><input type="text" class="input-note" placeholder="Lý do..."></td>
            </tr>
        `;
        tbody.insertAdjacentHTML('beforeend', rowHTML);
    });

    // Gán sự kiện tính toán độ lệch
    document.querySelectorAll(".actual-qty").forEach(input => {
        input.addEventListener("input", function() {
            const row = this.closest("tr");
            const sysQty = parseInt(row.querySelector(".sys-qty").innerText);
            let actualQty = parseInt(this.value) || 0;
            const diff = actualQty - sysQty;
            const diffSpan = row.querySelector(".diff-display");
            diffSpan.innerText = diff > 0 ? "+" + diff : diff;
            diffSpan.className = "diff-display " + (diff > 0 ? "diff-positive" : (diff < 0 ? "diff-negative" : "diff-zero"));
        });
    });
    document.getElementById("auditModal").style.display = "flex";
}

async function loadData() {
    try {
        const response = await fetch('api_khohang.php');
        const data = await response.json();
        
        if (data.error) {
            alert("Lỗi: " + data.error);
            return;
        }

        inventoryDb = data;
        renderInventoryTable();
        renderBatchTable();
    } catch (error) {
        console.error("Không thể tải dữ liệu:", error);
    }
}

// ==========================================
// 4. KHỞI TẠO
// ==========================================
document.addEventListener("DOMContentLoaded", () => {
    loadData(); // Tải dữ liệu từ database

    // Gán các sự kiện nút bấm
    document.getElementById("btnOpenAudit")?.addEventListener("click", openAuditModal);
    document.getElementById("btnSaveAudit")?.addEventListener("click", () => {
        // Chèn hàm saveAudit của bạn vào đây, nhớ đổi productsDb thành inventoryDb
        alert("Tính năng lưu đang cập nhật!"); 
    });

    // Đóng Modal
    const closeAudit = () => document.getElementById("auditModal").style.display = "none";
    document.getElementById("closeAuditModal")?.addEventListener("click", closeAudit);
    document.getElementById("btnCancelAudit")?.addEventListener("click", closeAudit);
});