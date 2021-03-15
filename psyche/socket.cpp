#include "socket.h"

#include <unistd.h>

#include "context.h"
#include "channel.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace psyche;

Socket::Socket(Context* c)
    : fd_(socket(AF_INET,SOCK_STREAM, 0)), context_(c) {
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
        close();
        context_->remove_channel(fd_);
        fd_ = 0;
    }
}

void Socket::reset() {
    fd_ = 0;
}

Socket::Socket(Context* c, int fd)
    : fd_(fd), context_(c) {
    using namespace std::placeholders;
    c->add_channel(fd_);
    c->get_channel(fd_)->enable_reading();
    c->get_channel(fd_)->set_close_callback([this]() { this->handle_close(); });
}

void Socket::shutdown(int how) const {
    ::shutdown(fd_, how);
}

void Socket::bind(const Endpoint& ep) const {
    auto sockaddr = ep.sockaddr();
    ::bind(fd_, sockaddr_cast(sockaddr), sizeof(sockaddr));
}

void Socket::connect(const Endpoint& ep) const {
    auto sockaddr_ = ep.sockaddr();
    ::connect(fd_, sockaddr_cast(sockaddr_), sizeof(sockaddr_));
}

void Socket::listen(int backlog) const {
    ::listen(fd_, backlog);
}

std::unique_ptr<Socket> Socket::accept() const {
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    auto fd = accept4(fd_, sockaddr_cast(addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (fd == -1) {
        throw std::runtime_error(std::to_string(errno));
    }
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
