#include "log.h"
#include "logging.h"
#include <iostream>

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

//template <typename T>
//LogInfo& LogInfo::operator<<(const T& content) {
//	buf.push_back(content.begin(),content.end());
//	return *this;
//}


LogInfo& LogInfo::operator<<(const char* content) {
    return operator<<(std::string(content));
}

LogInfo& LogInfo::operator<<(char* content) {
    return operator<<(std::string(content));
}

LogInfo& LogInfo::operator<<(const std::string& content) {
    buf += content;
    return *this;
}

LogInfo::LogInfo(const LogLevel level, std::string filename, const int line)
    : buf(), logLevel_(level), sourcefile(std::move(filename)), line_(line) {
}

LogInfo::~LogInfo() {
    buf += "\n";
    if (logLevel_ >= print_level)
        std::cout << buf;
    buf = std::string(LogLevelName[logLevel_]) + sourcefile + ":"
        + std::to_string(line_) + "\t" + buf;
    if (logLevel_ >= log_level)
        logger.add_log(std::move(buf));
}
