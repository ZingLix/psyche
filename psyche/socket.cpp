#include "socket.h"
#include "context.h"
#include "channel.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "LogInfo.h"

using namespace psyche;

socket::socket(context * c)
	:fd_(::socket(AF_INET,SOCK_STREAM,0)),context_(c)
{
	using namespace std::placeholders;
	int opt = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
		static_cast<const void *>(&opt), sizeof(opt));
	c->add_channel(fd_);
	c->get_channel(fd_)->enableReading();
	c->get_channel(fd_)->setCloseCallback(std::bind(&socket::handleClose, this));
}

//psyche::socket::socket(socket&& soc) noexcept {
//	fd_ = soc.fd_;
//	context_ = soc.context_;
//	soc.reset();
//}

psyche::socket::~socket() {
	if(fd_!=0) {
		context_->remove_channel(fd_);
	}
	LOG_INFO << "socket " << fd_ << " destroyed.";
}

void psyche::socket::reset() {
	fd_ = 0;
}

socket::socket(context * c,int fd)
	: fd_(fd),context_(c)
{
	using namespace std::placeholders;
	c->add_channel(fd_);
	c->get_channel(fd_)->enableReading();

	c->get_channel(fd_)->setCloseCallback(std::bind(&socket::handleClose, this));

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

void socket::read(buffer& buffer, readCallback cb) {
	context_->get_channel(fd_)->enableReading();
	context_->set_read_callback(fd_, cb, &buffer);
}

void socket::write(buffer& buffer, writeCallback cb) {
	context_->get_channel(fd_)->enableWriting();
	context_->set_write_callback(fd_, cb, &buffer);
}

void socket::handleClose() {
	LOG_INFO << "socket " << fd_ << " closed.";
	close();
	context_->remove_channel(fd_);
	fd_ = 0;
	if (cb) cb();
}

void socket::setCloseCallback(std::function<void()> cb1) {
	/*context_->get_channel(fd_)->setCloseCallback([=](error_code ec, std::size_t length) {
		this->handleClose(ec, length);
		cb(ec);
		});*/
	cb = cb1;
}

void socket::updateBuffer(buffer& read_buffer, buffer& write_buffer) {
	context_->get_channel(fd_)->update_buffer(&read_buffer, &write_buffer);
}

