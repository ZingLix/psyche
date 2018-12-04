#pragma once
#include <ctime>
#include <vector>
#include <unistd.h>

namespace psyche
{

class buffer_basic
{
public:
	std::size_t read_fd(int fd);
	std::size_t write_fd(int fd);
};

template <class T>
class buffer;

template <>
class buffer<std::vector<char>> :public buffer_basic
{
public:
	buffer(std::vector<char>& buf) :vector_(buf) {}

	std::size_t read_fd(int fd) const {
		const std::size_t n = ::read(fd, &*vector_.begin(), vector_.size());
		return n;
	}

	std::size_t write_fd(int fd) const {
		const std::size_t n = ::write(fd, &*vector_.begin(), vector_.size());
		return n;
	}

private:
	std::vector<char>& vector_;
};

buffer(std::vector<char>)->buffer<std::vector<char>>;

}
