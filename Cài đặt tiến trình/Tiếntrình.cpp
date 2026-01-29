#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip> // De dinh dang output
#include <limits>  // De su dung numeric_limits
#include <string>  // De su dung string

using namespace std;

// --- Cau truc Process ---
struct Process {
    string id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int first_run_time; // Thoi gian bat dau xu ly lan dau tien
    bool is_completed;
    bool has_run_once; // Danh dau tien trinh da duoc CPU xu ly it nhat mot lan chua

    Process(string id, int at, int bt, int p = 0) :
        id(id), arrival_time(at), burst_time(bt), priority(p),
        remaining_burst_time(bt), completion_time(0), turnaround_time(0),
        waiting_time(0), first_run_time(-1), is_completed(false), has_run_once(false) {}

    Process() : id(""), arrival_time(0), burst_time(0), priority(0), remaining_burst_time(0),
                completion_time(0), turnaround_time(0), waiting_time(0),
                first_run_time(-1), is_completed(false), has_run_once(false) {}
};

// --- Cau truc cho bang Gantt ---
struct GanttEntry {
    string process_id;
    int start_time;
    int end_time;
};

// --- Ham hien thi ket qua ---
void displayResults(const vector<Process>& processes) {
    double total_ct = 0;
    double total_tat = 0;
    double total_wt = 0;
    double total_rt = 0; 
    int num_completed_processes = 0;

    // Chi tinh trung binh cho cac tien trinh thuc su duoc chay
    for (const auto& p : processes) {
        if (p.is_completed) { // Chi tinh cac tien trinh da hoan thanh
            total_ct += p.completion_time;
            total_tat += p.turnaround_time;
            total_wt += p.waiting_time;
            // Response Time chi co y nghia neu tien trinh da tung duoc chay
            if (p.has_run_once) {
                total_rt += (p.first_run_time - p.arrival_time);
            }
            num_completed_processes++;
        }
    }

    cout << "\n--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    cout << setw(10) << "Process ID" << setw(15) << "Arrival Time" << setw(15) << "Burst Time"
         << setw(17) << "Completion Time" << setw(17) << "Turnaround Time" << setw(15) << "Waiting Time"
         << setw(17) << "Response Time" << endl; 
    cout << "           (ID tien trinh) (Thoi gian den) (Thoi gian xu ly) (Thoi gian hoan thanh) (Thoi gian luu tru) (Thoi gian cho) (Thoi gian dap ung)" << endl;
    cout << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

    for (const auto& p : processes) {
        cout << setw(10) << p.id << setw(15) << p.arrival_time << setw(15) << p.burst_time
             << setw(17) << p.completion_time << setw(17) << p.turnaround_time << setw(15) << p.waiting_time;
        if (p.has_run_once) {
             cout << setw(17) << (p.first_run_time - p.arrival_time) << endl;
        } else {
             cout << setw(17) << "N/A" << endl; // Khong ap dung neu chua chay
        }
    }

    cout << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    if (num_completed_processes > 0) {
        cout << "Average Completion Time (Thoi gian hoan thanh trung binh): " << fixed << setprecision(2) << total_ct / num_completed_processes << endl;
        cout << "Average Turnaround Time (Thoi gian luu tru trung binh): " << fixed << setprecision(2) << total_tat / num_completed_processes << endl;
        cout << "Average Waiting Time (Thoi gian cho trung binh): " << fixed << setprecision(2) << total_wt / num_completed_processes << endl;
        cout << "Average Response Time (Thoi gian dap ung trung binh): " << fixed << setprecision(2) << total_rt / num_completed_processes << endl; 
    } else {
        cout << "Khong co tien trinh nao hoan thanh de tinh trung binh.\n";
    }
    cout << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
}

// --- Ham ve bang Gantt (da cai thien can chinh) ---
void drawGanttChart(const vector<GanttEntry>& gantt_chart) {
    if (gantt_chart.empty()) {
        cout << "\nKhong co du lieu de ve bang Gantt.\n";
        return;
    }

    cout << "\n--- Bang Gantt ---\n";

    string process_line = "";
    string time_line = "";
    string divider_line = "";

    // Bat dau bang Gantt o thoi gian 0
    time_line += "0"; 

    for (size_t i = 0; i < gantt_chart.size(); ++i) {
        const auto& entry = gantt_chart[i];
        
        // Xu ly khoang CPU ranh (idle time)
        if (i == 0 && entry.start_time > 0) { // Truong hop CPU ranh ngay tu dau
             string idle_label = "CPU Idle";
             int idle_block_width = max((int)idle_label.length() + 2, entry.start_time - 0 + 2); // Toi thieu 2 ky tu cach

             process_line += "|";
             process_line.append(idle_block_width - idle_label.length(), ' '); // Canh le
             process_line += idle_label;
             process_line.append(idle_block_width - idle_label.length() - (idle_block_width - idle_label.length()), ' '); // Canh le

             divider_line.append(idle_block_width + 1, '-'); // +1 cho dau |
             time_line.append(idle_block_width + 1 - to_string(0).length(), ' ');
             time_line += to_string(entry.start_time);
        } else if (i > 0 && entry.start_time > gantt_chart[i-1].end_time) { // CPU ranh giua cac tien trinh
            int idle_start = gantt_chart[i-1].end_time;
            int idle_end = entry.start_time;
            string idle_label = "CPU Idle";
            int idle_block_width = max((int)idle_label.length() + 2, idle_end - idle_start + 2);

            process_line += "|";
            process_line.append(idle_block_width - idle_label.length(), ' '); 
            process_line += idle_label;
            process_line.append(idle_block_width - idle_label.length() - (idle_block_width - idle_label.length()), ' '); 

            divider_line.append(idle_block_width + 1, '-');
            time_line.append(idle_block_width + 1 - to_string(idle_start).length(), ' ');
            time_line += to_string(idle_end);
        }

        // Xu ly khoang thuc thi cua tien trinh
        string proc_label = entry.process_id;
        int proc_block_width = max((int)proc_label.length() + 2, entry.end_time - entry.start_time + 2); // +2 de co khoang trong 2 ben

        process_line += "|";
        process_line.append(proc_block_width - proc_label.length(), ' '); 
        process_line += proc_label;
        process_line.append(proc_block_width - proc_label.length() - (proc_block_width - proc_label.length()), ' '); 
        
        divider_line.append(proc_block_width + 1, '-');
        time_line.append(proc_block_width + 1 - to_string(entry.start_time).length(), ' ');
        time_line += to_string(entry.end_time);
    }
    
    // Them ky tu ket thuc cho cac dong
    process_line += "|";
    divider_line += "-";
    time_line += ""; // Moc thoi gian cuoi cung da duoc them

    cout << divider_line << endl;
    cout << process_line << endl;
    cout << divider_line << endl;
    cout << time_line << endl;
    cout << "\n";
}


// --- 1. Thuat toan FCFS (First Come First Served) - Luon la doc quyen ---
void FCFS(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    int current_time = 0;
    vector<GanttEntry> gantt_chart;
    for (auto& p : processes) {
        if (current_time < p.arrival_time) {
            // CPU ranh
            // Do ham drawGanttChart tu xu ly idle time, khong can them vao gantt_chart o day
            current_time = p.arrival_time;
        }
        
        // Ghi nhan thoi gian bat dau chay lan dau tien
        if (!p.has_run_once) {
            p.first_run_time = current_time;
            p.has_run_once = true;
        }

        gantt_chart.push_back({p.id, current_time, current_time + p.burst_time}); 
        p.completion_time = current_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
        p.is_completed = true; // Danh dau hoan thanh
        current_time = p.completion_time;
    }
    displayResults(processes);
    drawGanttChart(gantt_chart);
}

// --- 2. Thuat toan Round Robin (RR) - Luon la khong doc quyen ---
void RoundRobin(vector<Process> processes, int quantum) {
    int n = processes.size();
    int current_time = 0;
    int completed_processes = 0;
    queue<int> ready_queue;
    vector<bool> in_ready_queue(n, false); // Theo doi tien trinh co trong hang doi hay khong
    vector<GanttEntry> gantt_chart;

    // Sap xep cac tien trinh theo thoi gian den ban dau
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    // Thoi gian cuoi cung mot tien trinh bi ngat
    vector<int> last_interruption_time(n, 0); 

    // Them cac tien trinh den tai thoi gian 0 vao hang doi
    for (int i = 0; i < n; ++i) {
        if (processes[i].arrival_time == 0) {
            ready_queue.push(i);
            in_ready_queue[i] = true;
            last_interruption_time[i] = 0; // Thoi gian den la thoi gian co the bat dau
        }
    }

    while (completed_processes < n) {
        if (!ready_queue.empty()) {
            int idx = ready_queue.front();
            ready_queue.pop();
            in_ready_queue[idx] = false;

            // Ghi nhan thoi gian bat dau chay lan dau tien
            if (!processes[idx].has_run_once) {
                processes[idx].first_run_time = current_time;
                processes[idx].has_run_once = true;
            }

            int start_slice_time = current_time;
            int time_processed = min(quantum, processes[idx].remaining_burst_time);
            current_time += time_processed;
            processes[idx].remaining_burst_time -= time_processed;

            gantt_chart.push_back({processes[idx].id, start_slice_time, current_time}); 

            // Them cac tien trinh moi den trong khoang thoi gian nay vao hang doi
            // Duyet qua tat ca tien trinh (khong chi bat dau tu idx+1) de dam bao tat ca tien trinh den duoc them vao
            for (int i = 0; i < n; ++i) {
                if (!processes[i].is_completed && !in_ready_queue[i] && processes[i].arrival_time <= current_time && i != idx) {
                    if (processes[i].arrival_time > start_slice_time) { // Neu moi den trong slice nay
                         ready_queue.push(i);
                         in_ready_queue[i] = true;
                    }
                }
            }
            // Them lai tien trinh hien tai neu chua hoan thanh.
            // Day la mot cach de dam bao tien trinh duoc them vao sau cac tien trinh moi den.
            if (processes[idx].remaining_burst_time > 0) {
                ready_queue.push(idx);
                in_ready_queue[idx] = true;
            } else {
                processes[idx].completion_time = current_time;
                processes[idx].turnaround_time = processes[idx].completion_time - processes[idx].arrival_time;
                processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
                processes[idx].is_completed = true;
                completed_processes++;
            }
        } else {
            // CPU ranh, tang thoi gian de cho tien trinh moi den
            current_time++;
            // Them cac tien trinh moi den (neu co) vao hang doi
            for (int i = 0; i < n; ++i) {
                if (!processes[i].is_completed && !in_ready_queue[i] && processes[i].arrival_time <= current_time) {
                    ready_queue.push(i);
                    in_ready_queue[i] = true;
                }
            }
        }
    }
    displayResults(processes);
    drawGanttChart(gantt_chart);
}

// --- Thuat toan Priority Scheduling (Do uu tien) ---
// Gia su: so uu tien nho hon thi uu tien cao hon.
void PriorityScheduling(vector<Process> processes, bool preemptive) {
    int n = processes.size();
    int current_time = 0;
    int completed_processes = 0;
    vector<GanttEntry> gantt_chart;
    string current_running_process_id = "";
    int current_process_start_time = 0;

    // Khong sap xep truoc theo arrival_time vi cac tien trinh co the den sau
    // va can duoc xem xet de lap lich

    while (completed_processes < n) {
        int best_process_idx = -1;
        int highest_priority = numeric_limits<int>::max(); // So nho hon = uu tien cao hon

        // Tim tien trinh co uu tien cao nhat den tai hoac truoc current_time va chua hoan thanh
        for (int i = 0; i < n; ++i) {
            if (!processes[i].is_completed && processes[i].arrival_time <= current_time) {
                if (processes[i].priority < highest_priority) {
                    highest_priority = processes[i].priority;
                    best_process_idx = i;
                }
            }
        }

        if (best_process_idx != -1) {
            Process& p = processes[best_process_idx];

            // Ghi nhan thoi gian bat dau chay lan dau tien
            if (!p.has_run_once) {
                p.first_run_time = current_time;
                p.has_run_once = true;
            }

            if (current_running_process_id != p.id) { // Tien trinh moi hoac chuyen doi tien trinh
                if (current_running_process_id != "") { // Ket thuc phan truoc cua tien trinh
                    gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
                }
                current_running_process_id = p.id;
                current_process_start_time = current_time;
            }

            if (!preemptive) { // Doc quyen (Non-Preemptive)
                current_time += p.remaining_burst_time;
                p.remaining_burst_time = 0; // Xy ly toan bo burst time
                p.completion_time = current_time;
                p.turnaround_time = p.completion_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time;
                p.is_completed = true;
                completed_processes++;
                // Ghi vao Gantt sau khi hoan thanh toan bo burst time
                if (current_running_process_id == p.id) {
                    gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
                    current_running_process_id = ""; 
                    current_process_start_time = 0;
                }
            } else { // Khong doc quyen (Preemptive)
                int time_processed_in_this_step = 1; // Xu ly tung don vi thoi gian

                current_time += time_processed_in_this_step;
                p.remaining_burst_time -= time_processed_in_this_step;

                if (p.remaining_burst_time == 0) {
                    p.completion_time = current_time;
                    p.turnaround_time = p.completion_time - p.arrival_time;
                    p.waiting_time = p.turnaround_time - p.burst_time;
                    p.is_completed = true;
                    completed_processes++;
                    // Ghi vao Gantt khi hoan thanh mot tien trinh bi ngat
                    if (current_running_process_id == p.id) {
                        gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
                        current_running_process_id = "";
                        current_process_start_time = 0;
                    }
                }
                // Neu tien trinh bi ngat, no se duoc xem xet lai o vong lap tiep theo
            }
        } else { 
            // CPU ranh, tang thoi gian cho den khi co tien trinh den
            current_time++;
        }
    }
    // Ghi phan con lai cua tien trinh dang chay (neu co) vao Gantt
    if (current_running_process_id != "") {
        gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
    }
    displayResults(processes);
    drawGanttChart(gantt_chart);
}

// --- Thuat toan SJF (Shortest Job First - Thoi gian xu ly ngan nhat) ---
// SJF non-preemptive (Doc quyen)
void SJF_NonPreemptive(vector<Process> processes) {
    int n = processes.size();
    int current_time = 0;
    int completed_processes = 0;
    vector<GanttEntry> gantt_chart;

    // Khong sap xep truoc theo arrival_time vi cac tien trinh co the den sau

    while (completed_processes < n) {
        int best_process_idx = -1;
        int shortest_burst = numeric_limits<int>::max();

        // Tim tien trinh co burst time ngan nhat den tai hoac truoc current_time va chua hoan thanh
        for (int i = 0; i < n; ++i) {
            if (!processes[i].is_completed && processes[i].arrival_time <= current_time) {
                if (processes[i].burst_time < shortest_burst) {
                    shortest_burst = processes[i].burst_time;
                    best_process_idx = i;
                }
            }
        }

        if (best_process_idx != -1) {
            Process& p = processes[best_process_idx];
            int start_time_slot = current_time;

            // Ghi nhan thoi gian bat dau chay lan dau tien
            if (!p.has_run_once) {
                p.first_run_time = current_time;
                p.has_run_once = true;
            }

            current_time += p.burst_time;
            p.remaining_burst_time = 0;
            p.completion_time = current_time;
            p.turnaround_time = p.completion_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            p.is_completed = true;
            completed_processes++;
            gantt_chart.push_back({p.id, start_time_slot, current_time});
        } else {
            current_time++; // CPU ranh
        }
    }
    displayResults(processes);
    drawGanttChart(gantt_chart);
}

// SRTF (Shortest Remaining Time First - Thoi gian xu ly con lai ngan nhat) - Preemptive
void SRTF_Preemptive(vector<Process> processes) {
    int n = processes.size();
    int current_time = 0;
    int completed_processes = 0;
    vector<GanttEntry> gantt_chart;
    string current_running_process_id = "";
    int current_process_start_time = 0;

    // Khong sap xep truoc theo arrival_time

    while (completed_processes < n) {
        int best_process_idx = -1;
        int shortest_remaining_time = numeric_limits<int>::max();

        // Tim tien trinh co remaining burst time ngan nhat den tai hoac truoc current_time va chua hoan thanh
        for (int i = 0; i < n; ++i) {
            if (!processes[i].is_completed && processes[i].arrival_time <= current_time) {
                if (processes[i].remaining_burst_time < shortest_remaining_time) {
                    shortest_remaining_time = processes[i].remaining_burst_time;
                    best_process_idx = i;
                }
            }
        }

        if (best_process_idx != -1) {
            Process& p = processes[best_process_idx];

            // Ghi nhan thoi gian bat dau chay lan dau tien
            if (!p.has_run_once) {
                p.first_run_time = current_time;
                p.has_run_once = true;
            }

            if (current_running_process_id != p.id) { // Tien trinh moi hoac chuyen doi tien trinh
                if (current_running_process_id != "") { // Ket thuc phan truoc cua tien trinh
                    gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
                }
                current_running_process_id = p.id;
                current_process_start_time = current_time;
            }

            int time_processed_in_this_step = 1; // Xu ly tung don vi thoi gian

            current_time += time_processed_in_this_step;
            p.remaining_burst_time -= time_processed_in_this_step;

            if (p.remaining_burst_time == 0) {
                p.completion_time = current_time;
                p.turnaround_time = p.completion_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time;
                p.is_completed = true;
                completed_processes++;
                // Ghi vao Gantt khi hoan thanh mot tien trinh bi ngat
                if (current_running_process_id == p.id) {
                    gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
                    current_running_process_id = "";
                    current_process_start_time = 0;
                }
            }
        } else { 
            current_time++; // CPU ranh
        }
    }
    // Ghi phan con lai cua tien trinh dang chay (neu co) vao Gantt
    if (current_running_process_id != "") {
        gantt_chart.push_back({current_running_process_id, current_process_start_time, current_time});
    }
    displayResults(processes);
    drawGanttChart(gantt_chart);
}


// --- Ham chinh (main) ---
int main() {
    int choice;
    do {
        cout << "\n--- Chon thuat toan lap lich CPU ---\n";
        cout << "1. FCFS (First Come First Served)\n";
        cout << "2. RR (Round Robin)\n";
        cout << "3. Theo do uu tien (Preemptive/Non-Preemptive)\n";
        cout << "4. Thoi gian xu ly ngan nhat (SJF/SRTF) - Lua chon chung\n";
        cout << "5. SJF (Shortest Job First - Doc quyen)\n";
        cout << "6. SRTF (Shortest Remaining Time First - Khong doc quyen)\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "Nhap lua chon cua ban: ";
        cin >> choice;

        if (choice == 0) {
            cout << "Thoat chuong trinh.\n";
            break;
        }

        if (choice < 1 || choice > 6) {
            cout << "Lua chon khong hop le. Vui long thu lai.\n";
            continue;
        }

        int num_processes;
        cout << "\nNhap so luong tien trinh: ";
        cin >> num_processes;

        vector<Process> processes_data;
        for (int i = 0; i < num_processes; ++i) {
            string id = "P" + to_string(i + 1);
            int at, bt, p = 0;
            cout << "\nNhap thong tin cho Tien trinh " << id << ":\n";
            cout << "  Arrival Time (Thoi gian den): ";
            cin >> at;
            cout << "  Burst Time (Thoi gian xu ly): ";
            cin >> bt;
            if (choice == 3) {
                cout << "  Priority (Do uu tien - so nho hon = uu tien cao hon): ";
                cin >> p;
            }
            
            processes_data.emplace_back(id, at, bt, p);
        }

        // Reset trang thai cua cac tien trinh truoc moi lan chay thuat toan moi
        // Dam bao ket qua doc lap giua cac lan chay
        for (auto& p : processes_data) {
            p.remaining_burst_time = p.burst_time;
            p.completion_time = 0;
            p.turnaround_time = 0;
            p.waiting_time = 0;
            p.first_run_time = -1; 
            p.is_completed = false;
            p.has_run_once = false; 
        }

        switch (choice) {
            case 1:
                cout << "\n--- Dang chay FCFS ---\n";
                FCFS(processes_data);
                break;
            case 2: {
                cout << "\n--- Dang chay Round Robin ---\n";
                int quantum;
                cout << "Nhap Time Quantum: ";
                cin >> quantum;
                RoundRobin(processes_data, quantum);
                break;
            }
            case 3: {
                cout << "\n--- Dang chay Priority Scheduling ---\n";
                int sub_choice_priority;
                cout << "  Chon che do cho Priority Scheduling:\n";
                cout << "  1. Doc quyen (Non-Preemptive)\n";
                cout << "  2. Khong doc quyen (Preemptive)\n";
                cout << "  Nhap lua chon phu: ";
                cin >> sub_choice_priority;
                if (sub_choice_priority == 1) {
                    PriorityScheduling(processes_data, false);
                } else if (sub_choice_priority == 2) {
                    PriorityScheduling(processes_data, true);
                } else {
                    cout << "Lua chon phu khong hop le. Vui long chon 1 hoac 2.\n";
                }
                break;
            }
            case 4: {
                cout << "\n--- Dang chay Thoi gian xu ly ngan nhat ---\n";
                int sub_choice_sjf_srtf;
                cout << "  Chon thuat toan:\n";
                cout << "  1. SJF (Shortest Job First - Doc quyen)\n";
                cout << "  2. SRTF (Shortest Remaining Time First - Khong doc quyen)\n";
                cout << "  Nhap lua chon phu: ";
                cin >> sub_choice_sjf_srtf;
                if (sub_choice_sjf_srtf == 1) {
                    SJF_NonPreemptive(processes_data);
                } else if (sub_choice_sjf_srtf == 2) {
                    SRTF_Preemptive(processes_data);
                } else {
                    cout << "Lua chon phu khong hop le. Vui long chon 1 hoac 2.\n";
                }
                break;
            }
            case 5:
                cout << "\n--- Dang chay SJF (Shortest Job First - Doc quyen) ---\n";
                SJF_NonPreemptive(processes_data);
                break;
            case 6:
                cout << "\n--- Dang chay SRTF (Shortest Remaining Time First - Khong doc quyen) ---\n";
                SRTF_Preemptive(processes_data);
                break;
        }
        cout << "\nNhan Enter de tiep tuc...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get(); // Tam dung de nguoi dung xem ket qua
    } while (choice != 0);

    return 0;
}
