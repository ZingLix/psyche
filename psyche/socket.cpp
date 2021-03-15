#include "socket.h"
#include "context.h"
#include "channel.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>

using namespace psyche;

socket::socket(context * c)
    :fd_(::socket(AF_INET,SOCK_STREAM,0)),context_(c)
{
    using namespace std::placeholders;
    int opt = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
        static_cast<const void *>(&opt), sizeof(opt));
    c->add_channel(fd_);
    c->get_channel(fd_)->enable_reading();
    c->get_channel(fd_)->set_close_callback(std::bind(&socket::handle_close, this));
}

//psyche::socket::socket(socket&& soc) noexcept {
//	fd_ = soc.fd_;
//	context_ = soc.context_;
//	soc.reset();
//}

psyche::socket::~socket() {
    if(fd_!=0) {
        close();
        context_->remove_channel(fd_);
        fd_ = 0;
    }
}

void psyche::socket::reset() {
    fd_ = 0;
}

socket::socket(context * c,int fd)
    : fd_(fd),context_(c)
{
    using namespace std::placeholders;
    c->add_channel(fd_);
    c->get_channel(fd_)->enable_reading();
    c->get_channel(fd_)->set_close_callback(std::bind(&socket::handle_close, this));
}

void socket::shutdown(int how) const {
    ::shutdown(fd_, how);
}

void socket::bind(const endpoint& ep) const{
    auto sockaddr_ = ep.sockaddr();
    ::bind(fd_, sockaddr_cast(sockaddr_), sizeof(sockaddr_));
}

void socket::connect(const endpoint& ep) const {
    auto sockaddr_ = ep.sockaddr();
    ::connect(fd_, sockaddr_cast(sockaddr_), sizeof(sockaddr_));
}

void socket::listen(int backlog) const {
    ::listen(fd_, backlog);
}

std::unique_ptr<psyche::socket> socket::accept() const {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    auto fd = ::accept4(fd_, sockaddr_cast(addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(fd == -1) {
        throw std::runtime_error(std::to_string(errno));
    }
    return std::make_unique<socket>(context_, fd);
}


sockaddr* socket::sockaddr_cast(sockaddr_in& sock_in) {
    return reinterpret_cast<sockaddr*>(&sock_in);
}

sockaddr* socket::sockaddr_cast(sockaddr_in* sock_in)  {
    return reinterpret_cast<sockaddr*>(sock_in);
}

endpoint socket::local_endpoint() const {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    getsockname(fd_, sockaddr_cast(addr), &len);
    return endpoint{ addr };
}

endpoint socket::peer_endpoint() const {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    getpeername(fd_, sockaddr_cast(addr), &len);
    return endpoint{ addr };
}

void socket::close() const {
    ::close(fd_);
}

void socket::read(BufferImpl& buffer, ReadCallback cb) {
    context_->get_channel(fd_)->enable_reading();
    context_->set_read_callback(fd_, cb);
}

void socket::write(BufferImpl& buffer, WriteCallback cb) {
    context_->get_channel(fd_)->enable_writing();
    context_->set_write_callback(fd_, cb);
}

void socket::handle_close() {
    if (cb) cb();
}

void socket::set_close_callback(std::function<void()> cb) {
    this->cb = cb;
}

void psyche::socket::enable_read() {
    context_->get_channel(fd_)->enable_reading();
}

void psyche::socket::disable_read() {
    context_->get_channel(fd_)->disable_reading();
}

void psyche::socket::disable_write() {
    context_->get_channel(fd_)->disable_writing();
}
