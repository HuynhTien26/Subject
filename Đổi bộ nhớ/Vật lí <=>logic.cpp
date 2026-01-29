#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cctype>

using namespace std;

const int MAX_SIZE = 100;

// ------------------------ PHAN DOAN ------------------------
struct Segment {
    int base;
    int limit;
};

Segment segmentTable[MAX_SIZE];
int segmentCount = 0;

// ------------------------ PHAN TRANG ------------------------
struct PageTableEntry {
    int page;
    int frame;
};

PageTableEntry pageTable[MAX_SIZE];
int pageCount = 0;
int pageSize = 1024; // Default

// ------------------------ HAM HO TRO ------------------------
bool isNumber(const string& s) {
    for (char c : s)
        if (!isdigit(c)) return false;
    return true;
}

int getIntInput(string message) {
    string input;
    int value;
    while (true) {
        cout << message;
        getline(cin, input);
        stringstream ss(input);
        if (ss >> value) return value;
        cout << "  Loi: Vui long nhap mot so nguyen!\n";
    }
}

int parsePageSize(string str) {
    for (auto& c : str) c = toupper(c);

    if (str.find("KB") != string::npos) {
        str = str.substr(0, str.find("KB"));
        return stoi(str) * 1024;
    } else if (str.find("B") != string::npos) {
        str = str.substr(0, str.find("B"));
        return stoi(str);
    } else {
        return stoi(str);
    }
}

// ------------------------ PHAN DOAN ------------------------
void nhapBangPhanDoan() {
    segmentCount = getIntInput("Nhap so luong doan (toi da 100): ");
    for (int i = 0; i < segmentCount; i++) {
        cout << "\nDoan " << i << ":\n";
        segmentTable[i].base = getIntInput("  Nhap base (co so): ");
        segmentTable[i].limit = getIntInput("  Nhap limit (do dai): ");
    }
}

void logicToPhysical_Segment() {
    int soLuong;
    cout << "\nNhap so luong dia chi logic can chuyen doi (dang segment.offset): ";
    cin >> soLuong;
    cin.ignore();

    for (int i = 0; i < soLuong; i++) {
        float diaChiLogic;
        cout << "\nDia chi logic thu " << (i + 1) << " (vi du: 2.100): ";
        cin >> diaChiLogic;

        int segmentIndex = (int)floor(diaChiLogic);
        int offset = round((diaChiLogic - segmentIndex) * 1000);

        if (segmentIndex < 0 || segmentIndex >= segmentCount) {
            cout << "  Loi: Doan khong ton tai!\n";
            continue;
        }

        if (offset < 0 || offset >= segmentTable[segmentIndex].limit) {
            cout << "  Loi: Offset vuot qua gioi han cua doan!\n";
        } else {
            int diaChiVatLy = segmentTable[segmentIndex].base + offset;
            cout << "  => Dia chi vat ly: " << diaChiVatLy << endl;
        }
    }
}

void physicalToLogic_Segment() {
    int soLuong = getIntInput("\nNhap so luong dia chi vat ly can kiem tra: ");
    for (int i = 0; i < soLuong; i++) {
        int diaChi;
        cout << "\nDia chi vat ly thu " << (i + 1) << ": ";
        cin >> diaChi;

        bool timThay = false;
        for (int j = 0; j < segmentCount; j++) {
            int base = segmentTable[j].base;
            int limit = segmentTable[j].limit;

            if (diaChi >= base && diaChi < base + limit) {
                int offset = diaChi - base;
                cout << "  => Dia chi logic: " << j << "." << offset << endl;
                timThay = true;
                break;
            }
        }

        if (!timThay) cout << "  Dia chi khong thuoc bat ky doan nao.\n";
    }
}

// ------------------------ PHAN TRANG ------------------------
void nhapBangPhanTrang() {
    string kichThuoc;
    cout << "Nhap kich thuoc moi trang (vi du: 4 KB hoac 1024 B): ";
    getline(cin, kichThuoc);
    pageSize = parsePageSize(kichThuoc);
    cout << "=> Kich thuoc moi trang: " << pageSize << " B\n";

    pageCount = getIntInput("\nNhap so luong trang (toi da 100): ");
    for (int i = 0; i < pageCount; i++) {
        cout << "\nTrang " << i << ":\n";

        int page = getIntInput("  Nhap so trang (page): ");
        int frame;
        while (true) {
            cout << "  Nhap frame tuong ung (-1 neu chua gan): ";
            string input;
            getline(cin, input);
            if (input == "-1") {
                frame = -1;
                break;
            }
            stringstream ss(input);
            if (ss >> frame) break;
            cout << "  Loi: Vui long nhap mot so nguyen!\n";
        }

        pageTable[i].page = page;
        pageTable[i].frame = frame;
    }
}

void logicToPhysical_Paging() {
    int soLuong = getIntInput("\nNhap so luong dia chi logic can chuyen doi: ");
    for (int i = 0; i < soLuong; i++) {
        int diaChiLogic;
        cout << "\nDia chi logic thu " << (i + 1) << ": ";
        cin >> diaChiLogic;

        int page = diaChiLogic / pageSize;
        int offset = diaChiLogic % pageSize;

        bool found = false;
        for (int j = 0; j < pageCount; j++) {
            if (pageTable[j].page == page) {
                if (pageTable[j].frame == -1) {
                    cout << "  Trang chua duoc gan frame.\n";
                } else {
                    int diaChiVatLy = pageTable[j].frame * pageSize + offset;
                    cout << "  => Dia chi vat ly: " << diaChiVatLy << endl;
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "  Trang khong ton tai trong bang phan trang.\n";
    }
}

void physicalToLogic_Paging() {
    int soLuong = getIntInput("\nNhap so luong dia chi vat ly can chuyen doi: ");
    for (int i = 0; i < soLuong; i++) {
        int diaChiVatLy;
        cout << "\nDia chi vat ly thu " << (i + 1) << ": ";
        cin >> diaChiVatLy;

        int frame = diaChiVatLy / pageSize;
        int offset = diaChiVatLy % pageSize;

        bool found = false;
        for (int j = 0; j < pageCount; j++) {
            if (pageTable[j].frame == frame) {
                int diaChiLogic = pageTable[j].page * pageSize + offset;
                cout << "  => Dia chi logic: " << diaChiLogic << endl;
                found = true;
                break;
            }
        }
        if (!found) cout << "  Frame khong ton tai trong bang phan trang.\n";
    }
}

// ------------------------ MAIN ------------------------
int main() {
    cout << "=== CHUONG TRINH CHUYEN DOI DIA CHI ===\n";
    cout << "Chon che do:\n";
    cout << "1. Phan doan (Segment)\n";
    cout << "2. Phan trang (Paging)\n";
    cout << "Lua chon: ";
    int cheDo;
    cin >> cheDo;
    cin.ignore();

    if (cheDo == 1) {
        nhapBangPhanDoan();

        cout << "\nChuyen doi:\n";
        cout << "1. Logic -> Vat ly\n";
        cout << "2. Vat ly -> Logic\n";
        cout << "Lua chon: ";
        int lc;
        cin >> lc;
        cin.ignore();

        if (lc == 1) logicToPhysical_Segment();
        else if (lc == 2) physicalToLogic_Segment();
        else cout << "Lua chon khong hop le!\n";

    } else if (cheDo == 2) {
        nhapBangPhanTrang();

        cout << "\nChuyen doi:\n";
        cout << "1. Logic -> Vat ly\n";
        cout << "2. Vat ly -> Logic\n";
        cout << "Lua chon: ";
        int lc;
        cin >> lc;
        cin.ignore();

        if (lc == 1) logicToPhysical_Paging();
        else if (lc == 2) physicalToLogic_Paging();
        else cout << "Lua chon khong hop le!\n";

    } else {
        cout << "Lua chon khong hop le!\n";
    }

    cout << "\n=== Ket thuc ===\n";
    return 0;
}
