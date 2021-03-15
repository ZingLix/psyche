#include "buffer.h"

#include <unistd.h>

psyche::BufferImpl::BufferImpl()
    : buffer_(k_initial_size_),
      begin_(k_prepend_size_),
      end_(k_prepend_size_) {
}

psyche::BufferImpl::BufferImpl(BufferImpl&& other) noexcept {
    buffer_ = std::move(other.buffer_);
    begin_ = other.begin_;
    end_ = other.end_;
}

const char* psyche::BufferImpl::start() {
    return &*buffer_.begin() + begin_;
}

const char* psyche::BufferImpl::end() {
    return &*buffer_.begin() + end_;
}

std::string psyche::BufferImpl::retrieve(std::size_t num) {
    if (num > current_size()) num = current_size();
    std::string tmp(buffer_.begin() + begin_, buffer_.begin() + begin_ + num);
    begin_ += num;
    check_index();
    return tmp;
}

std::string psyche::BufferImpl::retrieve_all() {
    std::string tmp(buffer_.begin() + begin_, buffer_.begin() + end_);
    index_init();
    return tmp;
}

std::string psyche::BufferImpl::retrieve_until(const std::string& str) {
    const std::string tmp(buffer_.begin() + begin_, buffer_.begin() + end_);
    const auto pos = tmp.find(str);
    if (pos == std::string::npos) return std::string();
    return retrieve(pos + str.length());
}


void psyche::BufferImpl::check_index() {
    if (begin_ == end_) index_init();
}

void psyche::BufferImpl::index_init() {
    begin_ = end_ = k_prepend_size_;
}

size_t psyche::BufferImpl::free_size() const {
    return buffer_.capacity() - end_;
}

size_t psyche::BufferImpl::current_size() const {
    return end_ - begin_;
}

size_t psyche::BufferImpl::read_fd(const int fd) {
    char buf[1024];
    const size_t n = read(fd, buf, sizeof buf);
    if (n != static_cast<size_t>(-1) && n != 0) {
        append(std::string(buf, buf + n));
        check_index();
    }
    return n;
}

size_t psyche::BufferImpl::write_fd(const int fd) {
    const size_t n = write(fd, &*buffer_.begin() + begin_, end_ - begin_);
    begin_ += n;
    check_index();
    return n;
}

void psyche::BufferImpl::append(const std::string& str) {
    if (free_size() <= str.length()) {
        std::copy(buffer_.begin() + begin_, buffer_.begin() + end_, buffer_.begin() + k_prepend_size_);
        if (buffer_.capacity() - current_size() - k_prepend_size_ < str.length()) {
            buffer_.resize(str.length() + end_ - begin_ + k_prepend_size_);
        }
    }
    //LOG_INFO << buffer_.capacity();
    std::copy(str.begin(), str.end(), buffer_.begin() + end_);
    end_ += str.length();
    //begin_ = 0;
}

void psyche::BufferImpl::prepend(const std::string& str) {
    if (buffer_.capacity() - current_size() < str.length()) {
        buffer_.reserve(str.length() + buffer_.capacity());
    }
    if (str.length() > begin_) {
        std::copy(str.begin() + begin_, str.begin() + end_, buffer_.begin() + str.length());
        end_ = current_size() + str.length();
        begin_ = str.length();
    }
    std::copy(str.begin(), str.end(), buffer_.begin() + begin_ - str.length());
    begin_ -= str.length();
}

size_t psyche::BufferWrapper::available() const {
    return buffer_->current_size();
}

std::string psyche::BufferWrapper::retrieve(std::size_t num) const {
    return buffer_->retrieve(num);
}

std::string psyche::BufferWrapper::retrieve_all() const {
    return buffer_->retrieve_all();
}

std::string psyche::BufferWrapper::retrieve_until(const std::string& str) const {
    return buffer_->retrieve_until(str);
}
