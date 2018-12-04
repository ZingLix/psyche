#pragma once
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <condition_variable>

class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};

	using Buffer = std::string;

	const size_t LOG_BUFFER_SIZE_LIMIT = 500;

	Logger();
	~Logger();

	template<typename T>
	void write(const T& content);

	void addLog(Buffer&& buf) {
		std::lock_guard<std::mutex> lock(buf_mutex_);
		buf1.push_back(buf);
		if (buf1.size() > LOG_BUFFER_SIZE_LIMIT) cv.notify_one();
	}

	void loop();

private:
	int fd;
	std::thread thread_;
	std::vector<Buffer> buf1, buf2;
	bool looping_;
	std::mutex buf_mutex_;
	std::condition_variable cv;
};


extern Logger logger;
extern Logger::LogLevel logLevel;
extern Logger::LogLevel printLevel;
