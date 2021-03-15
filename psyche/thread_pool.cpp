#include "thread_pool.h"

psyche::ThreadPool::ThreadPool(size_t thread_nums): stop_(false) {
    for (int i = 0; i < thread_nums; i++) {
        threads_.emplace_back(&ThreadPool::worker_thread, this);
    }
}

psyche::ThreadPool::~ThreadPool() {
    stop_force();
    for (auto& t : threads_) t.join();
}

void psyche::ThreadPool::stop_force() {
    stop_ = true;
    cv.notify_all();
}

void psyche::ThreadPool::stop() {
    while (!task_queue_.empty());
    stop_force();
}

void psyche::ThreadPool::worker_thread() {
    while (!stop_) {
        Task t;
        {
            std::unique_lock<std::mutex> lock(q_mutex_);
            cv.wait(lock, [&] { return stop_ || !task_queue_.empty(); });
            if (!task_queue_.empty()) {
                t = std::move(task_queue_.front());
                task_queue_.pop();
            }
        }
        t();
    }
}
