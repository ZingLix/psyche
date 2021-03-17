#include "socket.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "context.h"
#include "channel.h"
#include "log.h"
#include "exception.h"

using namespace psyche;

Socket::Socket(Context* c)
    : fd_(socket(AF_INET,SOCK_STREAM, 0)), context_(c) {
    check_errno(fd_);
    using namespace std::placeholders;
    int opt = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
               static_cast<const void*>(&opt), sizeof(opt));
    c->add_channel(fd_);
    c->get_channel(fd_)->enable_reading();
    c->get_channel(fd_)->set_close_callback([this]() { this->handle_close(); });
}

//psyche::Socket::Socket(Socket&& soc) noexcept {
//	fd_ = soc.fd_;
//	context_ = soc.context_;
//	soc.reset();
//}

Socket::~Socket() {
    if (fd_ != 0) {
        context_->remove_channel(fd_);
        close();
        fd_ = 0;
    }
}

void Socket::reset() {
    fd_ = 0;
}

Socket::Socket(Context* c, const int fd)
    : fd_(fd), context_(c) {
    c->add_channel(fd_);
    c->get_channel(fd_)->enable_reading();
    c->get_channel(fd_)->set_close_callback([this]() { this->handle_close(); });
}

void Socket::shutdown(int how) const {
    check_errno(::shutdown(fd_, how));
}

void Socket::bind(const Endpoint& ep) const {
    auto sockaddr = ep.sockaddr();
    check_errno(::bind(fd_, sockaddr_cast(sockaddr), sizeof(sockaddr)));
}

void Socket::connect(const Endpoint& ep) const {
    auto sockaddr_ = ep.sockaddr();
    check_errno(::connect(fd_, sockaddr_cast(sockaddr_), sizeof(sockaddr_)));
}

void Socket::listen(int backlog) const {
    check_errno(::listen(fd_, backlog));
}

std::unique_ptr<Socket> Socket::accept() const {
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    auto fd = accept4(fd_, sockaddr_cast(addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    check_errno(fd);
    return std::make_unique<Socket>(context_, fd);
}


sockaddr* Socket::sockaddr_cast(sockaddr_in& sock_in) {
    return reinterpret_cast<sockaddr*>(&sock_in);
}

sockaddr* Socket::sockaddr_cast(sockaddr_in* sock_in) {
    return reinterpret_cast<sockaddr*>(sock_in);
}

Endpoint Socket::local_endpoint() const {
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    getsockname(fd_, sockaddr_cast(addr), &len);
    return Endpoint{addr};
}

Endpoint Socket::peer_endpoint() const {
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    getpeername(fd_, sockaddr_cast(addr), &len);
    return Endpoint{addr};
}

void Socket::close() const {
    ::close(fd_);
}

void Socket::read(BufferImpl& buffer, ReadCallback cb) const {
    context_->get_channel(fd_)->enable_reading();
    context_->set_read_callback(fd_, std::move(cb));
}

void Socket::write(BufferImpl& buffer, WriteCallback cb) const {
    context_->get_channel(fd_)->enable_writing();
    context_->set_write_callback(fd_, std::move(cb));
}

void Socket::handle_close() const {
    if (cb_) cb_();
}

void Socket::set_close_callback(std::function<void()> cb) {
    this->cb_ = std::move(cb);
}

void Socket::enable_read() const {
    context_->get_channel(fd_)->enable_reading();
}

void Socket::disable_read() const {
    context_->get_channel(fd_)->disable_reading();
}

void Socket::disable_write() const {
    context_->get_channel(fd_)->disable_writing();
}
