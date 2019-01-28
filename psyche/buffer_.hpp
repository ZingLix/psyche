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
	buffer_impl() 
		:buffer_(kInitialSize), 
		begin_(kPrependSize),
		end_(kPrependSize) 
	{ }

	buffer_impl(buffer_impl&& other) noexcept{
		buffer_ = std::move(other.buffer_);
		begin_ = other.begin_;
		end_ = other.end_;
	}

	const char* start() {
		return &*buffer_.begin() + begin_;
	}

	const char* end() {
		return &*buffer_.begin() + end_;
	}

	std::string retrieve(std::size_t num=0) {
		if(num==0) {
			std::string tmp(buffer_.begin() + begin_, buffer_.begin() + end_);
			indexInit();
			return tmp;
		}else {
			if (num > curSize()) num = curSize();
			std::string tmp(buffer_.begin() + begin_, buffer_.begin() + begin_+num);
			begin_ += num;
			checkIndex();
			return tmp;
		}
	}

	void checkIndex() {
		if (begin_ == end_) indexInit();
	}

	void indexInit() {
		begin_ = end_ = kPrependSize;
	}

	size_t freeSize() const {
		return buffer_.capacity() - end_;
	}

	size_t curSize() const {
		return end_ - begin_;
	}

	size_t readFd(int fd) {
		char buf[1024];
		size_t size=0;
		while(true) {
			size_t n = read(fd, buf, 1024);
			if (n ==-1||n==0) break;
			append(std::string(buf,buf+n));
			size += n;
		}
		checkIndex();
		return size;
	}

	size_t writeFd(int fd) {
		size_t n = write(fd, &*buffer_.begin() + begin_, end_-begin_);
		begin_ += n;
		checkIndex();
		return n;
	}

	void append(const std::string& str) {
		if(freeSize()<str.length()) {
			std::copy(buffer_.begin() + begin_, buffer_.begin() + end_, buffer_.begin());
			if(buffer_.capacity()-curSize()<str.length()) {
				buffer_.reserve(str.length() + begin_ - end_);
			}
		}
		std::copy(str.begin(), str.end(), buffer_.begin() + end_  );
		end_ += str.length();
		//begin_ = 0;
	}

	void prepend(const std::string& str) {
		if(buffer_.capacity()-curSize()<str.length()) {
			buffer_.reserve(str.length() + buffer_.capacity());
		}
		if(str.length()>begin_) {
			std::copy(str.begin() + begin_, str.begin() + end_, buffer_.begin() + str.length());
			end_ = curSize() + str.length();
			begin_ = str.length();
		}
		std::copy(str.begin(), str.end(), buffer_.begin() + begin_ - str.length());
		begin_ -= str.length();
	}

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

	size_t available() const {
		return buffer_->curSize();
	}

	std::string retrieve(std::size_t num = 0) {
		return buffer_->retrieve(num);
	}

private:
	buffer_impl* buffer_;
};

using Buffer = buffer_wrapper;

}
