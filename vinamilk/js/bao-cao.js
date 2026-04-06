// ==========================================
// 1. DỮ LIỆU TOÀN CỤC
// ==========================================
let salesReportDb = [];
let currentChart = null;

// Hàm định dạng tiền tệ an toàn
const formatCurrency = (num) => {
    if (num === null || num === undefined) return "0₫";
    let number = parseFloat(num);
    if (isNaN(number)) number = 0;
    return number.toLocaleString('vi-VN') + "₫";
};

// ==========================================
// 2. RENDER BÁO CÁO TỔNG QUAN (chỉ doanh thu)
// ==========================================
function renderBusinessDashboard(dataArray) {
    let totalOrders = dataArray.length;
    let totalQty = 0;
    let totalRevenue = 0;
    
    dataArray.forEach(order => {
        // Ép kiểu số
        const qty = parseFloat(order.qty) || 0;
        const revenue = parseFloat(order.revenue) || 0;
        totalQty += qty;
        totalRevenue += revenue;
    });

    document.getElementById("totalOrders").innerText = totalOrders;
    document.getElementById("totalQty").innerText = totalQty.toLocaleString('vi-VN');
    document.getElementById("totalRevenue").innerText = formatCurrency(totalRevenue);
    
    // Ẩn các thẻ tổng vốn và lãi nếu có
    const costEl = document.getElementById("totalCost");
    const profitEl = document.getElementById("totalProfit");
    if (costEl) costEl.closest('.summary-card').style.display = "none";
    if (profitEl) profitEl.closest('.summary-card').style.display = "none";

    // Vẽ biểu đồ doanh thu theo ngày
    const chartDataMap = {};
    dataArray.forEach(order => {
        const date = order.date;
        const revenue = parseFloat(order.revenue) || 0;
        if (!chartDataMap[date]) chartDataMap[date] = 0;
        chartDataMap[date] += revenue;
    });
    const labels = Object.keys(chartDataMap).sort();
    const chartRev = labels.map(date => chartDataMap[date]);
    renderChart(labels, chartRev);
}

// ==========================================
// 3. RENDER BẢNG CÔNG NỢ (có nút xem chi tiết hóa đơn)
// ==========================================
function renderDebtTable(dataArray) {
    const tbody = document.getElementById("debtTableBody");
    tbody.innerHTML = "";
    
    const debtMap = {};
    dataArray.forEach(order => {
        const customer = order.customer;
        const revenue = parseFloat(order.revenue) || 0;
        const paid = parseFloat(order.paid) || 0;
        if (!debtMap[customer]) {
            debtMap[customer] = { totalBought: 0, totalPaid: 0, orders: [] };
        }
        debtMap[customer].totalBought += revenue;
        debtMap[customer].totalPaid += paid;
        debtMap[customer].orders.push({
            id: order.id,
            date: order.date,
            revenue: revenue,
            paid: paid
        });
    });

    let hasDebt = false;
    for (let customer in debtMap) {
        const data = debtMap[customer];
        const debt = data.totalBought - data.totalPaid;
        if (debt > 0) {
            hasDebt = true;
            const rowHTML = `
                <tr>
                    <td><strong>${customer}</strong></td>
                    <td>${formatCurrency(data.totalBought)}</td>
                    <td style="color: #0056b3;">${formatCurrency(data.totalPaid)}</td>
                    <td class="text-danger">${formatCurrency(debt)}</td>
                    <td><span class="badge-debt">Đang nợ</span></td>
                    <td><button class="btn-view-invoice" data-customer="${customer}" style="background:#17a2b8; border:none; padding:5px 10px; border-radius:5px; color:white; cursor:pointer;"><i class="fas fa-receipt"></i> Xem hóa đơn</button></td>
                </tr>
            `;
            tbody.insertAdjacentHTML('beforeend', rowHTML);
        }
    }

    if (!hasDebt) {
        tbody.innerHTML = `<tr><td colspan="6" class="text-center">Tuyệt vời! Không có ai nợ.</td></tr>`;
    }

    // Gắn sự kiện xem hóa đơn
    document.querySelectorAll('.btn-view-invoice').forEach(btn => {
        btn.removeEventListener('click', window.invoiceHandler);
        const handler = () => {
            const customer = btn.getAttribute('data-customer');
            showCustomerInvoices(customer, debtMap[customer].orders);
        };
        btn.addEventListener('click', handler);
        window.invoiceHandler = handler; // lưu để tránh trùng nếu cần
    });
}

// Hiển thị modal chi tiết hóa đơn của khách hàng
function showCustomerInvoices(customer, orders) {
    document.getElementById("modalCustomerName").innerText = customer;
    const tbody = document.getElementById("invoiceDetailBody");
    tbody.innerHTML = "";
    orders.forEach(inv => {
        const debt = inv.revenue - inv.paid;
        tbody.insertAdjacentHTML('beforeend', `
            <tr>
                <td>${inv.id}</td>
                <td>${inv.date}</td>
                <td>${formatCurrency(inv.revenue)}</td>
                <td>${formatCurrency(inv.paid)}</td>
                <td class="${debt > 0 ? 'text-danger' : 'text-success'}">${formatCurrency(debt)}</td>
            </tr>
        `);
    });
    document.getElementById("customerInvoiceModal").style.display = "flex";
}

// ==========================================
// 4. RENDER BẢNG DOANH THU THEO CỬA HÀNG (mục 3)
// ==========================================
function renderRevenueByCustomer(dataArray) {
    const customerMap = {};
    dataArray.forEach(order => {
        const customer = order.customer;
        const qty = parseFloat(order.qty) || 0;
        const revenue = parseFloat(order.revenue) || 0;
        if (!customerMap[customer]) {
            customerMap[customer] = { totalRevenue: 0, totalQty: 0, orderCount: 0 };
        }
        customerMap[customer].totalRevenue += revenue;
        customerMap[customer].totalQty += qty;
        customerMap[customer].orderCount += 1;
    });

    const tbody = document.getElementById("revenueByCustomerBody");
    tbody.innerHTML = "";
    for (let customer in customerMap) {
        const data = customerMap[customer];
        tbody.insertAdjacentHTML('beforeend', `
            <tr>
                <td>${customer}</td>
                <td>${data.orderCount}</td>
                <td>${data.totalQty}</td>
                <td><strong>${formatCurrency(data.totalRevenue)}</strong></td>
            </tr>
        `);
    }
    if (Object.keys(customerMap).length === 0) {
        tbody.innerHTML = `<tr><td colspan="4" class="text-center">Không có dữ liệu</td></tr>`;
    }
}

// ==========================================
// 5. BIỂU ĐỒ DOANH THU
// ==========================================
function renderChart(labels, revenueData) {
    const ctx = document.getElementById('revenueChart').getContext('2d');
    if (currentChart) currentChart.destroy();
    currentChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: labels,
            datasets: [{
                label: 'Doanh thu',
                data: revenueData,
                backgroundColor: 'rgba(40, 167, 69, 0.7)',
                borderRadius: 5
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                y: {
                    beginAtZero: true,
                    ticks: { callback: value => value.toLocaleString('vi-VN') + "đ" }
                }
            },
            plugins: {
                tooltip: { callbacks: { label: ctx => ctx.dataset.label + ': ' + ctx.parsed.y.toLocaleString('vi-VN') + "đ" } }
            }
        }
    });
}

// ==========================================
// 6. LỌC DỮ LIỆU THEO THÁNG/NĂM
// ==========================================
function filterData() {
    const selectedMonth = document.getElementById("filterMonth").value;
    const selectedYear = document.getElementById("filterYear").value;
    const searchMonth = `${selectedYear}-${selectedMonth}`;
    const filteredData = salesReportDb.filter(item => item.date.startsWith(searchMonth));
    renderBusinessDashboard(filteredData);
    renderDebtTable(filteredData);
    renderRevenueByCustomer(filteredData);
}

function resetFilter() {
    document.getElementById("filterMonth").value = "03";
    document.getElementById("filterYear").value = "2026";
    renderBusinessDashboard(salesReportDb);
    renderDebtTable(salesReportDb);
    renderRevenueByCustomer(salesReportDb);
}

// ==========================================
// 7. XUẤT EXCEL/PDF
// ==========================================
function exportToExcel() {
    const wb = XLSX.utils.book_new();
    const wsDebt = XLSX.utils.table_to_sheet(document.getElementById("debtTable"));
    XLSX.utils.book_append_sheet(wb, wsDebt, "Cong_No");
    const wsRevenue = XLSX.utils.table_to_sheet(document.getElementById("revenueByCustomerTable"));
    XLSX.utils.book_append_sheet(wb, wsRevenue, "Doanh_Thu_Theo_Cua_Hang");
    XLSX.writeFile(wb, "Bao_Cao_Vinamilk.xlsx");
}

function exportToPDF() {
    alert("Hệ thống sẽ mở hộp thoại In. Vui lòng chọn 'Save as PDF'.");
    window.print();
}

// ==========================================
// 8. KHỞI TẠO
// ==========================================
document.addEventListener("DOMContentLoaded", async () => {
    document.getElementById("filterMonth").value = "03";
    document.getElementById("filterYear").value = "2026";

    try {
        const response = await fetch('api_baocao.php');
        const data = await response.json();
        if (data.error) throw new Error(data.error);
        // Ép kiểu toàn bộ dữ liệu số từ API
        salesReportDb = data.map(item => ({
            ...item,
            qty: parseFloat(item.qty) || 0,
            cost: parseFloat(item.cost) || 0,
            revenue: parseFloat(item.revenue) || 0,
            paid: parseFloat(item.paid) || 0
        }));
        filterData();
    } catch (error) {
        console.error("Lỗi:", error);
        alert("Không thể tải dữ liệu báo cáo.");
    }

    document.getElementById("btnFilter")?.addEventListener("click", filterData);
    document.getElementById("btnResetFilter")?.addEventListener("click", resetFilter);
    document.getElementById("btnExportExcel")?.addEventListener("click", exportToExcel);
    document.getElementById("btnExportPDF")?.addEventListener("click", exportToPDF);

    // Đóng modal chi tiết hóa đơn
    const invoiceModal = document.getElementById("customerInvoiceModal");
    const closeInvoiceBtn = document.getElementById("closeInvoiceModal");
    
    // Đổi tên hàm thành hideInvoiceModal để tránh xung đột với ID html
    const hideInvoiceModal = () => {
        if (invoiceModal) {
            invoiceModal.style.display = "none";
        }
    };

    if (closeInvoiceBtn) {
        closeInvoiceBtn.addEventListener("click", hideInvoiceModal);
    } else {
        console.warn("Không tìm thấy nút đóng modal hóa đơn");
    }

    // Đóng khi click ra ngoài overlay
    window.addEventListener("click", (e) => {
        if (e.target === invoiceModal) {
            hideInvoiceModal();
        }
    });
});