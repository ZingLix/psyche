#pragma once

#include "logging.h"

class LogInfo
{
public:
    using LogLevel = Logger::LogLevel;

    LogInfo(LogLevel level, std::string filename, int line);

    template <typename T>
    LogInfo& operator<<(const T&content);

    LogInfo& operator<<(const char * content);
    LogInfo& operator<<(char * content);
    LogInfo& operator<<(const std::string& content);
    ~LogInfo();

private:
    std::string buf;
    LogLevel logLevel_;
    std::string sourcefile;
    int line_;
};

template <typename T>
LogInfo& LogInfo::operator<<(const T& content) {
    this->operator<<(std::to_string(content));
    return *this;
}

constexpr const char * getfilename(const char * str) {
    int idx = 0;
    for(int i=0;str[i]!='\0';++i) {
        if (str[i] == '/') idx = i;
    }
    return str + idx+1;
}


#define LOG_TRACE LogInfo(Logger::TRACE,getfilename(__FILE__), __LINE__)
#define LOG_DEBUG LogInfo(Logger::DEBUG,getfilename(__FILE__), __LINE__)
#define LOG_INFO LogInfo(Logger::INFO,getfilename(__FILE__), __LINE__)
#define LOG_WARN LogInfo(Logger::WARN,getfilename(__FILE__), __LINE__)
#define LOG_ERROR LogInfo(Logger::ERROR,getfilename(__FILE__), __LINE__)
#define LOG_FATAL LogInfo(Logger::FATAL,getfilename(__FILE__), __LINE__)


