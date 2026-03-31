//
// Created by halim on 5.03.2026.
//

#ifndef MY_STL_MULTI_TASKING_H
#define MY_STL_MULTI_TASKING_H
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>


#pragma comment(lib, "ws2_32.lib")
namespace nsq {
    class ThreadPool {
    public:
        ThreadPool(size_t threads) {
            for(size_t i = 0; i < threads; ++i) {
                workers.emplace_back([this] {
                    while(true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                            if(this->stop && this->tasks.empty()) return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }
                        task();
                    }
                });
            }
        }

        void enqueue(std::function<void()> f) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.push(f);
            }
            condition.notify_one();
        }

        ~ThreadPool() {
            { std::unique_lock<std::mutex> lock(queue_mutex); stop = true; }
            condition.notify_all();
            for(std::thread &worker: workers) worker.join();
        }

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop = false;
    };
}
#endif //MY_STL_MULTI_TASKING_H