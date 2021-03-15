#include "logging.h"
#include <fcntl.h>
#include <unistd.h>
#include <numeric>
#include <mutex>

Logger logger;
Logger::LogLevel log_level(Logger::DEBUG);
Logger::LogLevel print_level(Logger::DEBUG);

Logger::Logger(): thread_(&Logger::loop, this) {
    auto time = std::time(nullptr);
    auto* const res = std::gmtime(&time);
    char filename[50];
    std::strftime(filename, sizeof(filename), "%Y%m%d%H%M%S.log", res);
    fd = open(filename, O_RDWR | O_APPEND | O_CREAT, 0777);
    looping_ = true;
    thread_.detach();
}

Logger::~Logger() {
    close(fd);
}

void Logger::add_log(Buffer&& buf) {
    std::lock_guard<std::mutex> lock(buf_mutex_);
    buf1.push_back(buf);
    if (buf1.size() > LOG_BUFFER_SIZE_LIMIT) cv.notify_one();
}

void Logger::loop() {
    using namespace std::chrono_literals;
    while (looping_) {
        {
            std::unique_lock<std::mutex> lk(buf_mutex_);
            if (buf1.size() < LOG_BUFFER_SIZE_LIMIT) {
                cv.wait_for(lk, 3s);
            }
            buf1.swap(buf2);
        }
        std::string entire_log;
        for (const auto& buf : buf2) {
            /*std::string log;
            for (auto str : buf) {
                log += str + " ";
            }*/
            entire_log += buf;
        }
        buf2.clear();
        ::write(fd, entire_log.c_str(), entire_log.length());
    }
}
