#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex resourceA;
std::mutex resourceB;

void simulateWork(const std::string& task, int ms) {
    std::cout << task << " working for " << ms << " ms\n";
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(ms)) {
        std::this_thread::yield(); // Simulated cooperative multitasking
    }
}

void lowPriorityTask1() {
    std::lock(resourceA, resourceB);
    std::lock_guard<std::mutex> lockA(resourceA, std::adopt_lock);
    std::lock_guard<std::mutex> lockB(resourceB, std::adopt_lock);

    std::cout << "LowPriorityTask1 has both resources\n";
    simulateWork("LowPriorityTask1", 200);
}

void lowPriorityTask2() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::lock(resourceA, resourceB);
    std::lock_guard<std::mutex> lockA(resourceA, std::adopt_lock);
    std::lock_guard<std::mutex> lockB(resourceB, std::adopt_lock);

    std::cout << "LowPriorityTask2 has both resources\n";
    simulateWork("LowPriorityTask2", 250);
}

void mediumPriorityTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    std::cout << "MediumPriorityTask starting CPU-bound work\n";
    simulateWork("MediumPriorityTask", 300);
    std::cout << "MediumPriorityTask done\n";
}

void highPriorityTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "HighPriorityTask attempting to lock ResourceA\n";
    std::lock_guard<std::mutex> lock(resourceA);
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
