#include <pthread.h>
#include <iostream>
#include <unistd.h>

pthread_mutex_t shared_resource;

void* lowPriorityTask(void*) {
    pthread_mutex_lock(&shared_resource);
    std::cout << "Low-priority task has the resource\n";
    sleep(3); 
    std::cout << "Low-priority task releasing resource\n";
    pthread_mutex_unlock(&shared_resource);
    return nullptr;
}

void* highPriorityTask(void*) {
    sleep(1); 
    std::cout << "High-priority task trying to acquire resource\n";
    pthread_mutex_lock(&shared_resource);
    std::cout << "High-priority task got the resource\n";
    pthread_mutex_unlock(&shared_resource);
    return nullptr;
}

int main() {
    pthread_mutex_init(&shared_resource, nullptr);

    pthread_t low, high;
    pthread_create(&low, nullptr, lowPriorityTask, nullptr);
    pthread_create(&high, nullptr, highPriorityTask, nullptr);

    pthread_join(low, nullptr);
    pthread_join(high, nullptr);

    pthread_mutex_destroy(&shared_resource); // Clean up
    return 0;
}
