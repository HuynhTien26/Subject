#include <iostream>
#include <vector>
#include <numeric>
#include <limits>
#include <cctype>
#include <string>

// Ham in ma tran
void printMatrix(const std::string& name, const std::vector<std::vector<int>>& matrix, int numProcesses, int numResources) {
    std::cout << "\n" << name << ":" << std::endl;
    std::cout << "     "; 
    for (int r = 0; r < numResources; ++r) {
        std::cout << (char)('A' + r) << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < numProcesses; ++i) {
        std::cout << "P" << i << ": ";
        for (int j = 0; j < numResources; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// in mot vector (work hoac available)
void printVector(const std::string& name, const std::vector<int>& vec, int numResources) {
    std::cout << "\n" << name << ":" << std::endl;
    for (int r = 0; r < numResources; ++r) {
        std::cout << (char)('A' + r) << " ";
    }
    std::cout << std::endl;
    for (int r = 0; r < numResources; ++r) {
        std::cout << vec[r] << " ";
    }
    std::cout << std::endl;
}

// In lich su work
void printWorkHistory(const std::string& name, const std::vector<std::vector<int>>& workHistory, 
                      const std::vector<int>& safeSequence, int numResources, int numProcesses) {
    std::cout << "\n" << name << ":" << std::endl;
    std::cout << "Step ";
    // in tieu de cot tai nguyen (A, B, C...)
    for (int r = 0; r < numResources; ++r) {
        std::cout << (char)('A' + r) << " ";
    }
    std::cout << " | Process Completed" << std::endl;
    
    // in trang thai work ban dau
    std::cout << "Init: ";
    for (int val : workHistory[0]) { 
        std::cout << val << " ";
    }
    std::cout << " | (Initial Available)" << std::endl;

    // in cac buoc work con lai kem theo 
    for (size_t i = 1; i < workHistory.size(); ++i) {
        std::cout << "  " << i << ": ";
        for (int val : workHistory[i]) {
            std::cout << val << " ";
        }
        // hien thi tien trinh da hoan thanh o buoc nay
        if (i - 1 < safeSequence.size()) {
            std::cout << " | P" << safeSequence[i-1] << " completed";
        }
        std::cout << std::endl;
    }
}


// doc so nguyen an toan
int getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cout << "Dau vao khong hop le. Vui long nhap mot so nguyen." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

//Kiem tra trang thai an toan
bool isSafe(
    const std::vector<int>& available,
    const std::vector<std::vector<int>>& max,
    const std::vector<std::vector<int>>& allocation,
    const std::vector<std::vector<int>>& need,
    int numProcesses,
    int numResources,
    std::vector<std::vector<int>>& work_history_out, 
    std::vector<std::vector<int>>& need_out,
    std::vector<int>& safe_sequence_out)
{
    std::vector<int> work = available;
    std::vector<bool> finish(numProcesses, false);
    std::vector<int> safeSequence; // chuoi an toan
    int count = 0;

    // luu trang thai work ban dau
    work_history_out.push_back(work);

    while (count < numProcesses)
    {
        bool found = false;
        for (int p = 0; p < numProcesses; ++p)
        {
            if (!finish[p])
            {
                bool canSatisfy = true;
                for (int r = 0; r < numResources; ++r)
                {
                    if (need[p][r] > work[r])
                    {
                        canSatisfy = false;
                        break;
                    }
                }

                if (canSatisfy)
                {
                    for (int r = 0; r < numResources; ++r)
                    {
                        work[r] += allocation[p][r];
                    }
                    finish[p] = true;
                    safeSequence.push_back(p);
                    work_history_out.push_back(work); // luu trang thai work sau khi P hoan thanh
                    found = true;
                    count++;
                }
            }
        }

        if (!found)
        {
            std::cout << "He thong o trang thai **KHONG AN TOAN!**" << std::endl;
            // Tr? v? chu?i an toàn dã tìm th?y d?n th?i di?m dó (n?u có)
            safe_sequence_out = safeSequence;
            need_out = need;
            return false;
        }
    }

    std::cout << "He thong o trang thai **AN TOAN!**" << std::endl;
    std::cout << "Chuoi an toan la: ";
    for (int i = 0; i < safeSequence.size(); ++i)
    {
        std::cout << "P" << safeSequence[i];
        if (i < safeSequence.size() - 1)
        {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;

    safe_sequence_out = safeSequence;
    need_out = need;
    return true;
}


int main()
{
    int numProcesses;
    int numResources;

    numProcesses = getIntInput("Nhap so luong tien trinh (P): ");
    if (numProcesses <= 0) {
        std::cerr << "Loi: So luong tien trinh phai lon hon 0." << std::endl;
        std::cout << "\nNhan Enter de thoat...";
        std::cin.get();
        return 1;
    }

    numResources = getIntInput("Nhap so luong loai tai nguyen (R): ");
    if (numResources <= 0) {
        std::cerr << "Loi: So luong loai tai nguyen phai lon hon 0." << std::endl;
        std::cout << "\nNhan Enter de thoat...";
        std::cin.get();
        return 1;
    }

    std::vector<int> available(numResources);
    std::vector<std::vector<int>> max(numProcesses, std::vector<int>(numResources));
    std::vector<std::vector<int>> allocation(numProcesses, std::vector<int>(numResources));
    std::vector<std::vector<int>> need(numProcesses, std::vector<int>(numResources));

    std::cout << "\n--- Nhap du lieu cho Available (Tai nguyen san co) ---" << std::endl;
    for (int r = 0; r < numResources; ++r)
    {
        available[r] = getIntInput("Nhap so luong tai nguyen loai " + std::string(1, (char)('A' + r)) + " san co: ");
        if (available[r] < 0) {
            std::cerr << "Loi: So luong tai nguyen khong the am." << std::endl;
            std::cout << "\nNhan Enter de thoat...";
            std::cin.get();
            return 1;
        }
    }

    std::cout << "\n--- Nhap du lieu cho Allocation (Tai nguyen da cap phat) ---" << std::endl;
    for (int i = 0; i < numProcesses; ++i)
    {
        std::cout << "Nhap tai nguyen da cap phat cho Tien trinh P" << i << ":\n";
        for (int j = 0; j < numResources; ++j)
        {
            allocation[i][j] = getIntInput("  Loai " + std::string(1, (char)('A' + j)) + ": ");
            if (allocation[i][j] < 0) {
                std::cerr << "Loi: So luong tai nguyen khong the am." << std::endl;
                std::cout << "\nNhan Enter de thoat...";
                std::cin.get();
                return 1;
            }
        }
    }

    std::cout << "\n--- Nhap du lieu cho Max (Nhu cau tai nguyen toi da) ---" << std::endl;
    for (int i = 0; i < numProcesses; ++i)
    {
        std::cout << "Nhap nhu cau tai nguyen TOI DA cho Tien trinh P" << i << ":\n";
        for (int j = 0; j < numResources; ++j)
        {
            max[i][j] = getIntInput("  Loai " + std::string(1, (char)('A' + j)) + ": ");
            if (max[i][j] < 0) {
                std::cerr << "Loi: So luong tai nguyen khong the am." << std::endl;
                std::cout << "\nNhan Enter de thoat...";
                std::cin.get();
                return 1;
            }
            if (max[i][j] < allocation[i][j]) {
                std::cerr << "Loi: Nhu cau toi da cua P" << i << " cho tai nguyen loai "
                          << (char)('A' + j) << " (" << max[i][j]
                          << ") khong the nho hon so luong da cap phat ("
                          << allocation[i][j] << ")." << std::endl;
                std::cout << "\nNhan Enter de thoat...";
                std::cin.get();
                return 1;
            }
        }
    }

    // Tinh bang NEED = MAX - ALLO
    for (int i = 0; i < numProcesses; ++i)
    {
        for (int j = 0; j < numResources; ++j)
        {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    // in ra bang MAX
    std::cout << "\n--- Tong quan du lieu HIEN TAI ---" << std::endl;
    printVector("Ma tran Available", available, numResources); // dung ham printVector cu
    printMatrix("Ma tran Max", max, numProcesses, numResources);
    printMatrix("Ma tran Allocation", allocation, numProcesses, numResources);
    printMatrix("Ma tran Need", need, numProcesses, numResources);


    // --- kiem tra an toan ban dau cua he thong
    std::cout << "\n--- Kiem tra an toan ban dau ---" << std::endl;
    std::vector<std::vector<int>> initial_work_history;
    std::vector<std::vector<int>> initial_need_snapshot;
    std::vector<int> initial_safe_sequence; // chuoi an toan
    
    if (isSafe(available, max, allocation, need, numProcesses, numResources, 
               initial_work_history, initial_need_snapshot, initial_safe_sequence)) {
        
        std::cout << "\nTrang thai cua Need sau kiem tra an toan ban dau (Need ban dau):" << std::endl;
        printMatrix("Need", initial_need_snapshot, numProcesses, numResources);
        std::cout << "\nLich su thay doi cua Work trong qua trinh tim chuoi an toan:" << std::endl;
        printWorkHistory("Work History", initial_work_history, initial_safe_sequence, numResources, numProcesses);

    } else {
        std::cout << "\nTrang thai cua Need (Need ban dau):" << std::endl;
        printMatrix("Need", initial_need_snapshot, numProcesses, numResources);
        std::cout << "\nLich su thay doi cua Work khi phat hien khong an toan:" << std::endl;
        printWorkHistory("Work History", initial_work_history, initial_safe_sequence, numResources, numProcesses);
    }
    
    char choice;
    do {
        std::cout << "\nBan co muon mot tien trinh yeu cau tai nguyen khong? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        if (std::tolower(choice) == 'y') {
            int requestingProcess;
            requestingProcess = getIntInput("Nhap so hieu tien trinh yeu cau (0 den " + std::to_string(numProcesses - 1) + "): P");

            if (requestingProcess < 0 || requestingProcess >= numProcesses) {
                std::cerr << "Loi: Tien trinh yeu cau khong hop le. Vui long nhap lai." << std::endl;
                continue; 
            }

            std::vector<int> request(numResources);
            std::cout << "Nhap yeu cau tai nguyen cho P" << requestingProcess << ":\n";
            bool re_enter_request = false;
            do {
                re_enter_request = false;
                for (int r = 0; r < numResources; ++r) {
                    request[r] = getIntInput("  Loai " + std::string(1, (char)('A' + r)) + ": ");
                    if (request[r] < 0) {
                        std::cerr << "Loi: So luong tai nguyen khong the am. Vui long nhap lai toan bo yeu cau cho tien trinh P" << requestingProcess << ".\n";
                        re_enter_request = true;
                        break; 
                    }
                }
            } while (re_enter_request);

            std::cout << "\nTien trinh P" << requestingProcess << " yeu cau: ";
            for(int r_val : request) {
                std::cout << r_val << " ";
            }
            std::cout << std::endl;

            // Yeu cau tai nguyen (Request Algorithm) ---
            bool requestExceedsNeed = false;
            for (int r = 0; r < numResources; ++r) {
                if (request[r] > need[requestingProcess][r]) {
                    requestExceedsNeed = true;
                    break;
                }
            }

            if (requestExceedsNeed) {
                std::cout << "Loi: Yeu cau vuot qua nhu cau toi da cua tien trinh P" << requestingProcess << std::endl;
            } else {
                bool requestExceedsAvailable = false;
                for (int r = 0; r < numResources; ++r) {
                    if (request[r] > available[r]) {
                        requestExceedsAvailable = true;
                        break;
                    }
                }

                if (requestExceedsAvailable) {
                    std::cout << "Tien trinh P" << requestingProcess << " phai cho (tai nguyen khong du san)." << std::endl;
                } else {
                    std::vector<int> newAvailable = available;
                    std::vector<std::vector<int>> newAllocation = allocation;
                    std::vector<std::vector<int>> newNeed = need;

                    for (int r = 0; r < numResources; ++r) {
                        newAvailable[r] -= request[r];
                        newAllocation[requestingProcess][r] += request[r];
                        newNeed[requestingProcess][r] -= request[r];
                    }

                    std::cout << "\nKiem tra trang thai sau khi gia dinh cap phat..." << std::endl;
                    std::vector<std::vector<int>> current_work_history;
                    std::vector<std::vector<int>> current_need_snapshot;
                    std::vector<int> current_safe_sequence;

                    if (isSafe(newAvailable, max, newAllocation, newNeed, numProcesses, numResources, 
                               current_work_history, current_need_snapshot, current_safe_sequence)) {
                        
                        std::cout << "Yeu cau tai nguyen cua P" << requestingProcess << " duoc **CHAP THUAN**." << std::endl;
                        available = newAvailable;
                        allocation = newAllocation;
                        need = newNeed;

                        std::cout << "\n--- Tong quan du lieu SAU KHI CAP PHAT ---" << std::endl;
                        std::cout << "Trang thai cua Need sau khi cap phat va kiem tra an toan:" << std::endl;
                        printMatrix("Need", current_need_snapshot, numProcesses, numResources);
                        std::cout << "\nLich su thay doi cua Work sau khi cap phat va kiem tra an toan:" << std::endl;
                        printWorkHistory("Work History", current_work_history, current_safe_sequence, numResources, numProcesses);
                    } else {
                        std::cout << "Yeu cau tai nguyen cua P" << requestingProcess << " bi **TU CHOI** (gay trang thai khong an toan)." << std::endl;
                       
                        std::cout << "\nTrang thai cua Need (gia dinh sau cap phat):" << std::endl;
                        printMatrix("Need", current_need_snapshot, numProcesses, numResources);
                        std::cout << "\nLich su thay doi cua Work khi phat hien khong an toan (gia dinh sau cap phat):" << std::endl;
                        printWorkHistory("Work History", current_work_history, current_safe_sequence, numResources, numProcesses);
                    }
                }
            }
        }
    } while (std::tolower(choice) == 'y');


    std::cout << "\nNhan Enter de thoat...";
    std::cin.get();
    return 0;
}
