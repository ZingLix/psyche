#pragma once
#include <ctime>
#include <vector>
#include <unistd.h>
#include <string>
namespace psyche
{
class buffer_impl
{
public:
	buffer_impl();
	buffer_impl(buffer_impl&& other) noexcept;

	const char* start();
	const char* end();

	std::string retrieve(std::size_t num);
	std::string retrieveAll();
    std::string retrieveUntil(std::string str);

	void checkIndex();
	void indexInit();

	size_t freeSize() const;
	size_t curSize() const;

	size_t readFd(int fd);
	size_t writeFd(int fd);

	void append(const std::string& str);
	void prepend(const std::string& str);

private:
	const std::size_t kInitialSize = 1024;
	const std::size_t kPrependSize = 8;

	std::vector<char> buffer_;
	std::size_t begin_;
	std::size_t end_;
};

class buffer_wrapper
{
public:
	buffer_wrapper(buffer_impl& b):buffer_(&b){}

	size_t available() const;
	std::string retrieve(std::size_t num) const;
	std::string retrieveAll() const;
    std::string retrieveUntil(std::string str) const;

private:
	buffer_impl* buffer_;
};

using Buffer = buffer_wrapper;

}
