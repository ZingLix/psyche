#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <future>
#include <type_traits>
#include <functional>
#include <memory>

namespace psyche
{
class ThreadPool
{
public:
    using Task = std::function<void()>;

    ThreadPool(size_t thread_nums = std::thread::hardware_concurrency());

    template<class F,class ...Args>
    auto execute(F&& f,Args&&... args) {
        using result_type = std::result_of_t<F(Args...)>;
        auto task = std::make_shared<std::packaged_task<result_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        task_queue_.emplace([=]()
        {
            task->operator()();
        });
        cv.notify_one();
        return task->get_future();
    }

    ~ThreadPool();

    void stop_force();
    void stop();

private:
    void worker_thread();

    std::atomic_bool stop_;
    std::mutex q_mutex_;
    std::queue<Task> task_queue_;
    std::condition_variable cv;
    std::vector<std::thread> threads_;
};
}
