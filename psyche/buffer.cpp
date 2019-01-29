#include "buffer.h"

psyche::buffer_impl::buffer_impl()
:buffer_(kInitialSize),
	begin_(kPrependSize),
	end_(kPrependSize) {
}

psyche::buffer_impl::buffer_impl(buffer_impl&& other) noexcept {
	buffer_ = std::move(other.buffer_);
	begin_ = other.begin_;
	end_ = other.end_;
}

const char* psyche::buffer_impl::start() {
	return &*buffer_.begin() + begin_;
}

const char* psyche::buffer_impl::end() {
	return &*buffer_.begin() + end_;
}

std::string psyche::buffer_impl::retrieve(std::size_t num) {
	if (num > curSize()) num = curSize();
	std::string tmp(buffer_.begin() + begin_, buffer_.begin() + begin_ + num);
	begin_ += num;
	checkIndex();
	return tmp;
}

std::string psyche::buffer_impl::retrieveAll() {
	std::string tmp(buffer_.begin() + begin_, buffer_.begin() + end_);
	indexInit();
	return tmp;
}


void psyche::buffer_impl::checkIndex() {
	if (begin_ == end_) indexInit();
}

void psyche::buffer_impl::indexInit() {
	begin_ = end_ = kPrependSize;
}

size_t psyche::buffer_impl::freeSize() const {
	return buffer_.capacity() - end_;
}

size_t psyche::buffer_impl::curSize() const {
	return end_ - begin_;
}

size_t psyche::buffer_impl::readFd(int fd) {
	char buf[1024];
	size_t n = read(fd, buf, sizeof buf);
	if (n != -1 && n != 0) {
		append(std::string(buf, buf + n));
		checkIndex();
	}
	return n;
}

size_t psyche::buffer_impl::writeFd(int fd) {
	size_t n = write(fd, &*buffer_.begin() + begin_, end_ - begin_);
	begin_ += n;
	checkIndex();
	return n;
}

void psyche::buffer_impl::append(const std::string& str) {
	if (freeSize() < str.length()) {
		std::copy(buffer_.begin() + begin_, buffer_.begin() + end_, buffer_.begin());
		if (buffer_.capacity() - curSize() < str.length()) {
			buffer_.reserve(str.length() + begin_ - end_);
		}
	}
	std::copy(str.begin(), str.end(), buffer_.begin() + end_);
	end_ += str.length();
	//begin_ = 0;
}

void psyche::buffer_impl::prepend(const std::string& str) {
	if (buffer_.capacity() - curSize() < str.length()) {
		buffer_.reserve(str.length() + buffer_.capacity());
	}
	if (str.length() > begin_) {
		std::copy(str.begin() + begin_, str.begin() + end_, buffer_.begin() + str.length());
		end_ = curSize() + str.length();
		begin_ = str.length();
	}
	std::copy(str.begin(), str.end(), buffer_.begin() + begin_ - str.length());
	begin_ -= str.length();
}

size_t psyche::buffer_wrapper::available() const {
	return buffer_->curSize();
}

std::string psyche::buffer_wrapper::retrieve(std::size_t num) const {
	return buffer_->retrieve(num);
}

std::string psyche::buffer_wrapper::retrieveAll() const {
	return buffer_->retrieveAll();
}
