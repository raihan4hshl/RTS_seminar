#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <chrono>

std::mutex system_mutex;
int current_ceiling = 0;


std::map<std::string, int> resource_ceiling = {
    {"ResourceA", 3},
    {"ResourceB", 4}
};


void accessResourceWithPCP(const std::string& task_name, const std::string& resource, int task_priority, int work_duration) {
    int res_ceil = resource_ceiling[resource];

    std::cout << task_name << " with priority " << task_priority << " attempting to access " << resource << " (Ceiling: " << res_ceil << ")\n";

    
    if (task_priority >= current_ceiling && task_priority >= res_ceil) {
        system_mutex.lock();
        current_ceiling = res_ceil;

        std::cout << task_name << " accessing " << resource << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(work_duration));
        std::cout << task_name << " releasing " << resource << "\n";

        current_ceiling = 0;
        system_mutex.unlock();
    }
    else {
        std::cout << task_name << " denied access due to ceiling protocol violation\n";
    }
}

int main() {
    // Launch simulated tasks with varying priorities
    std::thread low_priority([]() {
        accessResourceWithPCP("LowPriorityTask", "ResourceA", 2, 3);
        });

    std::thread high_priority([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Delay to simulate scheduling
        accessResourceWithPCP("HighPriorityTask", "ResourceA", 4, 1);
        });

    low_priority.join();
    high_priority.join();

    return 0;
}
