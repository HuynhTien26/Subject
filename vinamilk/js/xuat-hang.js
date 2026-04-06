// ==========================================
// 1. BIẾN TOÀN CỤC
// ==========================================
window.productsList = [];
window.customersList = [];
window.salesDb = [];

// ==========================================
// 2. CÁC HÀM GỌI API
// ==========================================
async function loadProducts() {
    try {
        const res = await fetch('api_xuathang.php?action=get_products');
        window.productsList = await res.json();
        if (document.getElementById("salesModal").style.display === "flex") {
            updateProductSelectOptions();
        }
    } catch (error) {
        console.error("Lỗi tải sản phẩm:", error);
    }
}

async function loadCustomers() {
    try {
        const res = await fetch('api_xuathang.php?action=get_customers');
        window.customersList = await res.json();
        const customerSelect = document.getElementById("customerName");
        if (customerSelect) {
            customerSelect.innerHTML = '<option value="">-- Chọn khách hàng --</option>';
            window.customersList.forEach(customer => {
                customerSelect.innerHTML += `<option value="${customer.MaKhach}">${customer.TenKhach}</option>`;
            });
        }
    } catch (error) {
        console.error("Lỗi tải khách hàng:", error);
    }
}

window.loadOrders = async function(filterStatus = 'processing') {
    try {
        // Lấy tất cả đơn hàng, lọc ở phía client để linh hoạt
        const url = `api_xuathang.php?action=get_orders&status=all`;
        const res = await fetch(url);
        window.salesDb = await res.json();
        window.renderSalesTable(filterStatus);
    } catch (error) {
        console.error("Lỗi tải đơn hàng:", error);
    }
};

// ==========================================
// 3. HÀM RENDER BẢNG LỊCH SỬ & TOGGLE THANH TOÁN
// ==========================================
const formatCurrency = (num) => {
    if (!num && num !== 0) return "0₫";
    const number = parseFloat(num);
    if (isNaN(number)) return "0₫";
    return number.toLocaleString('vi-VN') + "₫";
};

// Chuyển đổi trạng thái thanh toán (đã thu/chưa thu)
window.togglePaymentStatus = async function(orderId, currentStatus) {
    const newStatus = currentStatus == 1 ? 0 : 1;
    const confirmMsg = `Xác nhận chuyển trạng thái thanh toán thành ${newStatus == 1 ? 'Đã thu tiền' : 'Chưa thu tiền'}?`;
    if (confirm(confirmMsg)) {
        try {
            const res = await fetch('api_xuathang.php?action=update_payment_status', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ orderId: orderId, paid: newStatus })
            });
            const result = await res.json();
            if (result.success) {
                await window.loadOrders(document.getElementById("salesFilter").value);
            } else {
                alert("Lỗi: " + (result.error || "Không xác định"));
            }
        } catch (error) {
            alert("Không thể kết nối server.");
        }
    }
};

window.renderSalesTable = function(filterStatus) {
    const tbody = document.getElementById("salesTableBody");
    if (!tbody) return;
    tbody.innerHTML = "";
    
    let filtered = window.salesDb;
    
    // Lọc theo yêu cầu: processing hoặc unpaid (tất cả đơn chưa thanh toán)
    if (filterStatus !== "all") {
        if (filterStatus === 'processing') {
            filtered = window.salesDb.filter(order => order.TrangThai === 'Đang xử lý');
        } else if (filterStatus === 'unpaid') {
            filtered = window.salesDb.filter(order => order.DaThuTien == 0);
        }
    }

    if (filtered.length === 0) {
        tbody.innerHTML = `<tr><td colspan="7" class="text-center">Chưa có đơn hàng nào.</td></tr>`;
        return;
    }

    filtered.forEach(order => {
        let badge = "";
        let actionBtn = `<button class="btn-action-view" onclick="viewSalesOrder(${order.MaDon})"><i class="fas fa-eye"></i> Xem</button>`;

        // Hiển thị trạng thái và nút hành động dựa trên trạng thái giao hàng
        if (order.TrangThai === "Đang xử lý") {
            badge = `<span class="badge-processing">📝 Đang xử lý</span>`;
            actionBtn += `<button class="btn-print" onclick="shipOrder(${order.MaDon})"><i class="fas fa-truck"></i> Giao hàng</button>`;
        } else if (order.TrangThai === "Đang giao") {
            badge = `<span class="badge-shipping">🚚 Đang giao</span>`;
        } else {
            badge = `<span class="badge-delivered">✅ Đã giao</span>`;
        }

        // Nút toggle thanh toán
        const paymentStatusText = order.DaThuTien == 1 ? "✅ Đã thu tiền" : "⏳ Chưa thu";
        const paymentButton = `<button class="btn-payment-toggle" style="background:${order.DaThuTien == 1 ? '#28a745' : '#ffc107'}; color:white; border:none; padding:5px 10px; border-radius:5px; cursor:pointer;" onclick="togglePaymentStatus(${order.MaDon}, ${order.DaThuTien})">${paymentStatusText}</button>`;

        const rowHTML = `
            <tr>
                <td><strong>${order.MaDon}</strong></td>
                <td>${new Date(order.NgayBan).toLocaleDateString('vi-VN')}</td>
                <td>${order.TenKhach}</td>
                <td class="text-danger"><strong>${formatCurrency(order.TongTien)}</strong></td>
                <td>${badge}</td>
                <td>${paymentButton}</td>
                <td class="text-center">${actionBtn}</td>
            </tr>
        `;
        tbody.insertAdjacentHTML('beforeend', rowHTML);
    });
};

// ==========================================
// 4. LOGIC KIỂM TRA TỒN KHO & LẬP ĐƠN
// ==========================================
function updateProductSelectOptions() {
    const selects = document.querySelectorAll("#salesTableItems .prod-select");
    selects.forEach(select => {
        const currentValue = select.value;
        let options = '<option value="">-- Chọn sản phẩm --</option>';
        window.productsList.forEach(prod => {
            options += `<option value="${prod.MaHang}">${prod.TenHang} (${prod.DonViTinh})</option>`;
        });
        select.innerHTML = options;
        if (currentValue) select.value = currentValue;
    });
}

function addSalesRow(productId = null, quantity = 1, isReadonly = false) {
    const tbody = document.getElementById("salesTableItems");
    const tr = document.createElement("tr");
    
    let prodData = { MaHang: '', TenHang: '', SoLuongTonKho: 0, DonGia: 0 };
    if (productId && window.productsList.length) {
        prodData = window.productsList.find(p => p.MaHang == productId) || prodData;
    }
    
    tr.innerHTML = `
        <td>
            <select class="form-control prod-select" ${isReadonly ? 'disabled' : ''}>
                <option value="">-- Chọn sản phẩm --</option>
                ${window.productsList.map(p => `<option value="${p.MaHang}">${p.TenHang} (${p.DonViTinh})</option>`).join('')}
            </select>
        </td>
        <td><span class="stock-display ${prodData.SoLuongTonKho < 1 ? 'text-danger' : 'text-success'}">${prodData.SoLuongTonKho}</span></td>
        <td><span class="price-display">${formatCurrency(prodData.DonGia)}</span></td>
        <td><input type="number" class="form-control qty-input" min="1" value="${quantity}" ${isReadonly ? 'disabled' : ''}></td>
        <td><strong><span class="subtotal-display">${formatCurrency(quantity * prodData.DonGia)}</span></strong></td>
        <td class="text-center col-sales-action">
            <button class="btn-delete btn-remove-row" ${isReadonly ? 'disabled' : ''}><i class="fas fa-trash"></i></button>
        </td>
    `;
    
    if (productId) {
        tr.querySelector(".prod-select").value = productId;
    }
    
    if (!isReadonly) {
        tr.querySelector(".prod-select").addEventListener("change", function() {
            const product = window.productsList.find(p => p.MaHang == this.value);
            if (product) {
                const stockSpan = tr.querySelector(".stock-display");
                stockSpan.innerText = product.SoLuongTonKho;
                stockSpan.className = `stock-display ${product.SoLuongTonKho < 1 ? 'text-danger' : 'text-success'}`;
                tr.querySelector(".price-display").innerText = formatCurrency(product.DonGia);
                validateStockAndCalculate();
            } else {
                tr.querySelector(".stock-display").innerText = "0";
                tr.querySelector(".price-display").innerText = formatCurrency(0);
                validateStockAndCalculate();
            }
        });
        tr.querySelector(".qty-input").addEventListener("input", validateStockAndCalculate);
        tr.querySelector(".btn-remove-row").addEventListener("click", function() {
            tr.remove();
            validateStockAndCalculate();
        });
    }
    
    tbody.appendChild(tr);
    if (!isReadonly) validateStockAndCalculate();
}

function validateStockAndCalculate() {
    const rows = document.getElementById("salesTableItems").querySelectorAll("tr");
    let totalOrderValue = 0;
    let hasStockError = false;

    rows.forEach(row => {
        const productId = row.querySelector(".prod-select").value;
        const qtyInput = row.querySelector(".qty-input");
        const stockDisplay = row.querySelector(".stock-display");
        const subTotalDisplay = row.querySelector(".subtotal-display");
        
        let reqQty = parseInt(qtyInput.value) || 0;
        let product = window.productsList.find(p => p.MaHang == productId);
        let availableStock = product ? product.SoLuongTonKho : 0;
        let price = product ? product.DonGia : 0;

        if (!productId) {
            qtyInput.classList.add("input-error");
            stockDisplay.classList.add("text-danger");
            hasStockError = true;
        } else if (reqQty > availableStock) {
            qtyInput.classList.add("input-error");
            stockDisplay.classList.add("text-danger");
            hasStockError = true;
        } else {
            qtyInput.classList.remove("input-error");
            stockDisplay.classList.remove("text-danger");
            stockDisplay.classList.add("text-success");
        }

        let rowSubTotal = reqQty * price;
        subTotalDisplay.innerText = formatCurrency(rowSubTotal);
        totalOrderValue += rowSubTotal;
    });

    document.getElementById("orderTotalDisplay").innerHTML = `Tổng cộng: ${formatCurrency(totalOrderValue)}`;

    const btnProcess = document.getElementById("btnProcessOrder");
    const warningMsg = document.getElementById("stockWarning");
    
    if (hasStockError) {
        btnProcess.disabled = true;
        warningMsg.style.display = "block";
    } else {
        btnProcess.disabled = false;
        warningMsg.style.display = "none";
    }

    return { isValid: !hasStockError, total: totalOrderValue };
}

// ==========================================
// 5. KHỞI TẠO VÀ BẮT SỰ KIỆN
// ==========================================
document.addEventListener("DOMContentLoaded", async () => {
    await loadProducts();
    await loadCustomers();
    await window.loadOrders("processing");
    
    document.getElementById("salesFilter").value = "processing";
    document.getElementById("salesDate").valueAsDate = new Date();
    
    const salesModal = document.getElementById("salesModal");
    const closeModal = () => salesModal.style.display = "none";
    
    document.getElementById("btnCreateSalesOrder").addEventListener("click", () => {
        document.getElementById("salesFormTitle").innerText = "Lập Đơn Bán Hàng";
        document.getElementById("salesId").value = "";
        document.getElementById("salesDate").disabled = false;
        document.getElementById("customerName").disabled = false;
        document.getElementById("btnAddSalesRow").style.display = "inline-block";
        document.getElementById("salesActionButtons").style.display = "flex";
        document.getElementById("colSalesAction").style.display = "table-cell";
        document.getElementById("salesTableItems").innerHTML = "";
        addSalesRow();
        salesModal.style.display = "flex";
    });
    
    document.getElementById("btnAddSalesRow").addEventListener("click", () => addSalesRow());
    document.getElementById("closeSalesModal").addEventListener("click", closeModal);
    document.getElementById("btnCancelSales").addEventListener("click", closeModal);
    
    document.getElementById("btnProcessOrder").addEventListener("click", async function() {
        const validation = validateStockAndCalculate();
        if(!validation.isValid) return;
        
        const rows = document.getElementById("salesTableItems").querySelectorAll("tr");
        if(rows.length === 0) return alert("Vui lòng chọn sản phẩm!");
        
        const customerId = document.getElementById("customerName").value;
        if (!customerId) {
            alert("Vui lòng chọn khách hàng!");
            return;
        }
        
        const date = document.getElementById("salesDate").value;
        if (!date) {
            alert("Vui lòng chọn ngày xuất!");
            return;
        }
        
        const items = [];
        rows.forEach(row => {
            const productId = row.querySelector(".prod-select").value;
            const qty = parseInt(row.querySelector(".qty-input").value);
            items.push({ productId: productId, quantity: qty });
        });
        
        const payload = {
            customerId: customerId,
            date: date,
            items: items
        };
        
        const btn = document.getElementById("btnProcessOrder");
        btn.disabled = true;
        btn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Đang xử lý...';
        
        try {
            const res = await fetch('api_xuathang.php?action=create_order', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            const result = await res.json();
            if(result.success) {
                alert(`✅ LẬP HÓA ĐƠN THÀNH CÔNG! Mã đơn: ${result.orderId}`);
                closeModal();
                await window.loadOrders(document.getElementById("salesFilter").value);
            } else {
                alert("Lỗi: " + (result.error || "Không xác định"));
            }
        } catch (error) {
            console.error(error);
            alert("Không thể kết nối tới server.");
        } finally {
            btn.disabled = false;
            btn.innerHTML = '<i class="fas fa-file-invoice-dollar"></i> Lập hóa đơn & Trừ kho';
        }
    });
});

// ==========================================
// 6. CÁC HÀM TOÀN CỤC GỌI TỪ HTML
// ==========================================
window.viewSalesOrder = async function(orderId) {
    try {
        const res = await fetch(`api_xuathang.php?action=get_order_details&id=${orderId}`);
        const details = await res.json();
        if (details.error) {
            alert(details.error);
            return;
        }
        
        const order = window.salesDb.find(o => o.MaDon == orderId);
        if (!order) return;
        
        document.getElementById("salesFormTitle").innerText = `Chi tiết Đơn Bán: ${orderId}`;
        document.getElementById("salesId").value = orderId;
        document.getElementById("salesDate").value = order.NgayBan.split(' ')[0];
        document.getElementById("customerName").value = order.MaKhach;
        
        document.getElementById("salesTableItems").innerHTML = "";
        details.forEach(item => {
            addSalesRow(item.MaHang, item.SoLuong, true);
        });
        
        document.getElementById("salesDate").disabled = true;
        document.getElementById("customerName").disabled = true;
        document.getElementById("btnAddSalesRow").style.display = "none";
        document.getElementById("salesActionButtons").style.display = "none";
        document.getElementById("colSalesAction").style.display = "none";
        document.querySelectorAll(".col-sales-action").forEach(el => el.style.display = "none");
        document.getElementById("stockWarning").style.display = "none";
        document.getElementById("orderTotalDisplay").innerHTML = `Tổng cộng: ${formatCurrency(order.TongTien)}`;
        
        document.getElementById("salesModal").style.display = "flex";
    } catch (error) {
        console.error(error);
        alert("Không thể tải chi tiết đơn hàng.");
    }
};

window.shipOrder = async function(orderId) {
    if (confirm("Xác nhận đã giao hàng thành công?")) {
        try {
            // Chuyển trạng thái đơn thành "Đã hoàn thành", không thay đổi DaThuTien
            const res = await fetch('api_xuathang.php?action=update_order_status', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ orderId: orderId, status: 'delivered' })
            });
            const result = await res.json();
            if (result.success) {
                await window.loadOrders(document.getElementById("salesFilter").value);
            } else {
                alert("Lỗi: " + (result.error || "Không xác định"));
            }
        } catch (error) {
            alert("Không thể kết nối server.");
        }
    }
};