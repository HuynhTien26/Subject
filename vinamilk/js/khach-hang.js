// ==========================================
// 1. BIẾN TOÀN CỤC
// ==========================================
let customers = [];          // dữ liệu từ API
let isEditing = false;
let editId = null;           // lưu mã khách hàng (số nguyên)

// Elements DOM
const customerTableBody = document.getElementById("customerTableBody");
const customerModal = document.getElementById("customerModal");
const customerFormTitle = document.getElementById("customerFormTitle");

const inputId = document.getElementById("customerId");
const inputName = document.getElementById("customerName");
const inputPhone = document.getElementById("customerPhone");
const inputAddress = document.getElementById("customerAddress");
const inputNote = document.getElementById("customerNote");

// ==========================================
// 2. HÀM GỌI API
// ==========================================
async function loadCustomers() {
    try {
        const res = await fetch('api_khachhang.php?action=get_customers');
        const data = await res.json();
        if (data.error) throw new Error(data.error);
        customers = data;
        renderTable();
    } catch (error) {
        console.error("Lỗi tải khách hàng:", error);
        alert("Không thể tải danh sách khách hàng.");
    }
}

async function addCustomerToDB(customerData) {
    const res = await fetch('api_khachhang.php?action=add_customer', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(customerData)
    });
    return await res.json();
}

async function updateCustomerInDB(customerData) {
    const res = await fetch('api_khachhang.php?action=update_customer', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(customerData)
    });
    return await res.json();
}

// ==========================================
// 3. HÀM HIỂN THỊ BẢNG (không có cột công nợ)
// ==========================================
function renderTable() {
    customerTableBody.innerHTML = "";
    
    customers.forEach((cus, index) => {
        const row = document.createElement("tr");
        row.innerHTML = `
            <td><strong>${cus.id}</strong>
            <td>${cus.name}
            <td>${cus.phone}
            <td>${cus.address || ''}
            <td class="text-center">
                <button class="btn-action-view btn-edit" data-id="${cus.id}" data-index="${index}" title="Chỉnh sửa"><i class="fas fa-edit"></i></button>
            </td>
        `;
        customerTableBody.appendChild(row);
    });

    // Gắn sự kiện cho nút chỉnh sửa
    document.querySelectorAll(".btn-edit").forEach(btn => {
        btn.addEventListener("click", function() {
            const id = this.getAttribute("data-id");   // mã dạng "KHxxx"
            const index = parseInt(this.getAttribute("data-index"));
            openEditModal(index);
        });
    });
}

// ==========================================
// 4. XỬ LÝ MODAL
// ==========================================
function clearForm() {
    inputName.value = "";
    inputPhone.value = "";
    inputAddress.value = "";
    inputNote.value = "";
}

function openEditModal(index) {
    isEditing = true;
    const cus = customers[index];
    editId = parseInt(cus.id);   // lưu mã số (phần số) để gửi API

    customerFormTitle.innerText = "Chỉnh Sửa Thông Tin Khách Hàng";
    inputId.value = cus.id;
    inputName.value = cus.name;
    inputPhone.value = cus.phone;
    inputAddress.value = cus.address || "";
    inputNote.value = cus.note || "";

    customerModal.style.display = "flex";
}

function openAddModal() {
    isEditing = false;
    editId = null;
    customerFormTitle.innerText = "Thêm Khách Hàng Mới";
    inputId.value = "";  // sẽ được generate sau khi lưu
    clearForm();
    customerModal.style.display = "flex";
}

function closeModal() {
    customerModal.style.display = "none";
}

// ==========================================
// 5. LƯU DỮ LIỆU (GỌI API)
// ==========================================
async function saveCustomer() {
    if (inputName.value.trim() === "" || inputPhone.value.trim() === "") {
        alert("Vui lòng nhập tên và số điện thoại!");
        return;
    }

    const customerData = {
        name: inputName.value.trim(),
        phone: inputPhone.value.trim(),
        address: inputAddress.value.trim(),
        note: inputNote.value.trim()   // vẫn gửi note nhưng API sẽ bỏ qua
    };

    if (isEditing && editId) {
        customerData.id = editId;
        const result = await updateCustomerInDB(customerData);
        if (result.success) {
            alert("Cập nhật thành công!");
            await loadCustomers();
            closeModal();
        } else {
            alert("Lỗi: " + (result.error || "Không xác định"));
        }
    } else {
        const result = await addCustomerToDB(customerData);
        if (result.success) {
            alert("Thêm khách hàng thành công!");
            await loadCustomers();
            closeModal();
        } else {
            alert("Lỗi: " + (result.error || "Không xác định"));
        }
    }
}

// ==========================================
// 6. KHỞI TẠO VÀ SỰ KIỆN
// ==========================================
document.addEventListener("DOMContentLoaded", () => {
    loadCustomers();

    // Nút mở modal
    document.getElementById("btnOpenCustomerModal").addEventListener("click", openAddModal);
    document.getElementById("closeCustomerModal").addEventListener("click", closeModal);
    document.getElementById("btnCancelCustomer").addEventListener("click", closeModal);
    document.getElementById("btnSaveCustomer").addEventListener("click", saveCustomer);

    // Đóng modal khi click ra ngoài
    window.addEventListener("click", (e) => {
        if (e.target === customerModal) closeModal();
    });
});