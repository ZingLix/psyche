#include "socket.h"
#include <sys/socket.h>
#include <netinet/in.h>

using namespace psyche;

socket::socket()
	:fd_(::socket(AF_INET,SOCK_STREAM,0))
{
	int opt = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
		static_cast<const void *>(&opt), sizeof(opt));
}

socket::socket(int fd)
	: fd_(fd)
{
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

void socket::listen(int backlog = 1000) const {
	::listen(fd_, backlog);
}

psyche::socket socket::accept() const {
	sockaddr_in addr;
	socklen_t len = sizeof(addr);
	auto fd = ::accept4(fd_, sockaddr_cast(addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
	return socket(fd);
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

void socket::read(const buffer_basic& buffer, readCallback cb) {
	context_->set_read_callback(fd_, cb, buffer);
}

void psyche::socket::write(const buffer_basic& buffer, writeCallback cb) {
	context_->set_write_callback(fd_, cb, buffer);

}

