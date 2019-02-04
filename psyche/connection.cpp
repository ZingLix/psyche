#include "connection.h"
#include "channel.h"


void psyche::connection::send(std::string msg, sendCallback cb) {
	using namespace std::placeholders;
	send_callback_ = cb;
	write_buffer_->append(msg);
	soc_->write(*write_buffer_, std::bind(&connection::handleSend, this));
}

psyche::connection::~connection() {
	LOG_DEBUG << "connection " << soc_->fd() << " start to destroy.";
}

void psyche::connection::setReadCallback(recvCallback cb) {
	using namespace std::placeholders;
	soc_->read(*read_buffer_, std::bind(&connection::handleRecv, this));
	recv_callback_ = cb;
}

void psyche::connection::setWriteCallback(sendCallback cb) {
	using namespace std::placeholders;
//	soc_->write(*write_buffer_, std::bind(&connection::handleSend, this));
	send_callback_ = cb;
}

void psyche::connection::setCloseCallback(closeCallback cb) {
	using namespace std::placeholders;
	soc_->setCloseCallback(std::bind(&connection::handleClose, this));
	close_callback_ = cb;
}

void psyche::connection::handleRecv() {
	/*if (ec&&ec!=4) {
		if (recv_callback_) recv_callback_(ec, nullptr, 0);
		else throw;
	}*/
	auto n = read_buffer_->readFd(soc_->fd());
	if(n==0) {
		handleClose();
		return;
	}
	if (recv_callback_) recv_callback_(shared_from_this(), *read_buffer_);
}

void psyche::connection::handleSend() {
	//if(ec&&ec != 4) {
	//	if (send_callback_) send_callback_();
	//	else throw;
	//}
	write_buffer_->writeFd(soc_->fd());
	if (send_callback_) send_callback_(shared_from_this());
}

void psyche::connection::handleClose() {
	if(close_callback_)	close_callback_(shared_from_this());
	
	//recv_callback_ = nullptr;
	//send_callback_ = nullptr;
	//close_callback_ = nullptr;
	//error_callback_ = nullptr;
}

psyche::connection_wrapper::connection_wrapper(connection_ptr c): conn(c) {
}

psyche::connection_wrapper::connection_wrapper(const connection_wrapper& c)
	:conn(c.conn)
{
}

psyche::connection_wrapper::connection_wrapper(connection_wrapper&& c) noexcept
	:conn(std::move(c.conn))
{
	
}


void psyche::connection_wrapper::send(std::string msg) const {
	conn->send(msg, conn->getWriteCallback());
}

psyche::connection::connection(context& c, int fd): soc_(std::make_unique<socket>(&c, fd)),
                                                    read_buffer_(std::make_unique<buffer_impl>()),
                                                    write_buffer_(std::make_unique<buffer_impl>()) {
}

psyche::connection::connection(std::unique_ptr<socket>&& soc): soc_(std::move(soc))//,
                                                             //  read_buffer_(std::make_unique<buffer_impl>()),
                                                             //  write_buffer_(std::make_unique<buffer_impl>())
{
	read_buffer_ = std::make_unique<buffer_impl>();
	write_buffer_ = std::make_unique<buffer_impl>();
}

psyche::connection::connection(connection&& other) noexcept
	:soc_(std::move(other.soc_)),
	read_buffer_(std::move(other.read_buffer_)),
	write_buffer_(std::move(other.write_buffer_)),
	recv_callback_(other.recv_callback_),
	send_callback_(other.send_callback_)
{
}
