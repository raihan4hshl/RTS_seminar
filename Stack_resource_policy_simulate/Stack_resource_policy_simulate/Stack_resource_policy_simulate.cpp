#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <algorithm>

struct Resource {
    std::string name;
    int ceiling;
};

std::mutex system_mutex;
int system_ceiling = 0;

bool canExecute(int task_priority) {
    return task_priority >= system_ceiling;
}

void accessResource(const std::string& task_name, const Resource& res, int task_priority, int exec_time_ms) {
    std::lock_guard<std::mutex> lock(system_mutex);

    std::cout << task_name << " (priority " << task_priority << ") attempting to access " << res.name << " (ceiling " << res.ceiling << ")\n";

    if (canExecute(task_priority)) {
        int prev_ceiling = system_ceiling;
        system_ceiling = std::max(system_ceiling, res.ceiling);

        std::cout << task_name << " granted access to " << res.name << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));
        std::cout << task_name << " done with " << res.name << "\n";

        system_ceiling = prev_ceiling;
    }
    else {
        std::cout << task_name << " blocked by SRP (system ceiling = " << system_ceiling << ")\n";
    }
}

int main() {
    Resource sensor = { "Sensor", 3 };
    Resource actuator = { "Actuator", 5 };

    std::vector<std::thread> tasks;

    tasks.emplace_back(accessResource, "TaskA", sensor, 2, 1000);   // should be blocked
    tasks.emplace_back(accessResource, "TaskB", sensor, 4, 800);    // should succeed
    tasks.emplace_back(accessResource, "TaskC", actuator, 5, 600);  // should succeed
    tasks.emplace_back(accessResource, "TaskD", actuator, 3, 900);  // should be blocked

    for (auto& t : tasks) {
        t.join();
    }

    std::cout << "SRP simulation complete.\n";
    return 0;
}
