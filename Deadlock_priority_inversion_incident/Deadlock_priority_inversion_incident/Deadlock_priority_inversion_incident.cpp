#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex resourceA;
std::mutex resourceB;

void simulateWork(const std::string& task, int ms) {
    std::cout << task << " working for " << ms << " ms\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void lowPriorityTask1() {
    std::lock_guard<std::mutex> lockA(resourceA);
    std::cout << "LowPriorityTask1 has locked ResourceA\n";
    simulateWork("LowPriorityTask1", 200);

    std::cout << "LowPriorityTask1 attempting to lock ResourceB\n";
    std::lock_guard<std::mutex> lockB(resourceB);  // Risk of deadlock
    std::cout << "LowPriorityTask1 has both resources\n";
    simulateWork("LowPriorityTask1", 100);
}

void lowPriorityTask2() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Let Task1 start
    std::lock_guard<std::mutex> lockB(resourceB);
    std::cout << "LowPriorityTask2 has locked ResourceB\n";
    simulateWork("LowPriorityTask2", 250);

    std::cout << "LowPriorityTask2 attempting to lock ResourceA\n";
    std::lock_guard<std::mutex> lockA(resourceA);  // Risk of deadlock
    std::cout << "LowPriorityTask2 has both resources\n";
    simulateWork("LowPriorityTask2", 100);
}

void mediumPriorityTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    std::cout << "MediumPriorityTask starting CPU-bound work\n";
    simulateWork("MediumPriorityTask", 300);  // Blocks CPU, shows priority inversion
    std::cout << "MediumPriorityTask done\n";
}

void highPriorityTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "HighPriorityTask attempting to lock ResourceA\n";
    std::lock_guard<std::mutex> lock(resourceA);  // Likely blocked
    std::cout << "HighPriorityTask has locked ResourceA\n";
    simulateWork("HighPriorityTask", 100);
}

int main() {
    std::thread t1(lowPriorityTask1);
    std::thread t2(lowPriorityTask2);
    std::thread t3(mediumPriorityTask);
    std::thread t4(highPriorityTask);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "Simulation complete.\n";
    return 0;
}
