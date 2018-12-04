#include "LogInfo.h"
#include "logging.h"
#include <type_traits>
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


LogInfo & LogInfo::operator<<(const char * content)
{
	return operator<<(std::string(content));
}

LogInfo & LogInfo::operator<<(char * content)
{
	return operator<<(std::string(content));
}

LogInfo& LogInfo::operator<<(const std::string& content) {
	buf += content;
	return *this;
}

LogInfo::LogInfo(LogLevel level, std::string filename, int line)
:buf(),logLevel_(level),sourcefile(filename),line_(line){
}

LogInfo::~LogInfo() {
	buf += "\n";
	if (logLevel_ >= printLevel)
		std::cout << buf ;
	buf = std::string(LogLevelName[logLevel_]) + sourcefile + ":"
		+ std::to_string(line_) + "\t" + buf ;
	if (logLevel_ >= logLevel)
		logger.addLog(std::move(buf));
}
