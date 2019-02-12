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
template<class T>
class thread_safe_queue
{
public:
	template<typename ...Args>
	thread_safe_queue(Args... args) :queue_(std::forward(args...)) {}

	thread_safe_queue() = default;

	auto front() {
		std::lock_guard<std::mutex> lock_guard(mutex_);
		return queue_.front();
	}

	auto back() {
		std::lock_guard<std::mutex> lock_guard(mutex_);
		return queue_.back();
	}

	auto empty() {
		std::lock_guard<std::mutex> lock_guard(mutex_);
		return queue_.empty();
	}

	void push(T&& val) {
		std::lock_guard<std::mutex> lock_guard(mutex_);
		queue_.push(std::forward(val));
	}

	void emplace(T&& val) {
		std::lock_guard<std::mutex> lock_guard(mutex_);
		queue_.emplace(std::forward<T>(val));
	}

	bool pop(T& result) {
		std::lock_guard<std::mutex> lock_guard(mutex_);
		if(queue_.empty()) {
			return false;
		}else {
			result= queue_.front();
			queue_.pop();
			return true;
		}
	}

private:
	std::mutex mutex_;
	std::queue<T> queue_;
};

class ThreadPool
{
public:
	using Task = std::function<void()>;

	ThreadPool(size_t thread_nums=std::thread::hardware_concurrency())
		:stop_(false) 
	{
		for(int i=0;i<thread_nums;i++) {
			threads_.emplace_back(&ThreadPool::worker_thread,this);
		}
	}

	template<class F,class ...Args>
	auto Execute(F&& f,Args&&... args) {
		using result_type = std::result_of_t<F(Args...)>;
		auto task = std::make_shared<std::packaged_task<result_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		task_queue_.emplace([=]()
		{
			task->operator()();
		});
		return task->get_future();
	}

	~ThreadPool() {
		stop_force();
		for (auto&t : threads_) t.join();
	}

	void stop_force() { stop_ = true; }
	void stop() {
		while (!task_queue_.empty());
		stop_ = true;
	}

private:
	void worker_thread() {
		while (!stop_) {
			Task t;
			if(task_queue_.pop(t)) {
				t();
			}else {
				std::this_thread::yield();
			}
		}
	}

	std::atomic_bool stop_;
	thread_safe_queue<Task> task_queue_;
	std::vector<std::thread> threads_;
};
}
