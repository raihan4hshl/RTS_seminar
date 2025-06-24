#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

class NonPreemptiveScheduler {
private:
    std::mutex scheduler_mutex;

public:
    void executeTask(int task_id, int exec_time_ms) {
        std::lock_guard<std::mutex> lock(scheduler_mutex); 

        std::cout << "Task " << task_id << " STARTED (Non-Preemptive)\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));
        std::cout << "Task " << task_id << " FINISHED\n";
    }
};

void runTask(NonPreemptiveScheduler& scheduler, int task_id, int exec_time_ms) {
    scheduler.executeTask(task_id, exec_time_ms);
}

int main() {
    NonPreemptiveScheduler scheduler;

    std::vector<std::thread> task_threads;
    task_threads.emplace_back(runTask, std::ref(scheduler), 1, 2000);
    task_threads.emplace_back(runTask, std::ref(scheduler), 2, 1500);
    task_threads.emplace_back(runTask, std::ref(scheduler), 3, 1000);

    for (auto& t : task_threads) {
        t.join();
    }

    std::cout << "All tasks completed under non-preemptive protocol.\n";
    return 0;
}
