#pragma once
#include <ctime>
#include <vector>
#include <unistd.h>

namespace psyche
{
class buffer
{
public:
	buffer() 
		:buffer_(kInitialSize), 
		read_index_(kPrependSize),
		write_index_(kPrependSize) 
	{ }

	buffer(buffer&& other) noexcept{
		buffer_ = std::move(other.buffer_);
		read_index_ = other.read_index_;
		write_index_ = other.write_index_;
	}

	std::size_t readFd(int fd) {
		auto n= ::read(fd, &*buffer_.begin() + write_index_, buffer_.capacity() - write_index_);
		write_index_ += n;
		return n;
	}
	
	std::size_t writeFd(int fd) {
		auto n = ::write(fd, &*buffer_.begin() + read_index_, write_index_-read_index_);
		//read_index_ += n;
		checkIndex();
		return n;
	}

	const char* start() {
		return &*buffer_.begin() + read_index_;
	}

	const char* end() {
		return &*buffer_.begin() + write_index_;
	}

	void retrieve(std::size_t num) {
		read_index_ += num;
		checkIndex();
	}

	void checkIndex() {
		if (read_index_ == write_index_) read_index_ = write_index_ = kPrependSize;
	}

	void append(std::string str) {
		std::copy(str.begin(), str.end(), buffer_.begin() + write_index_);
	//	buffer_.insert(buffer_.begin()+write_index_, str.begin(), str.end());
		write_index_ += str.length();
	}
private:
	const std::size_t kInitialSize = 1024;
	const std::size_t kPrependSize = 8;

	std::vector<char> buffer_;
	std::size_t read_index_;
	std::size_t write_index_;
};

}
