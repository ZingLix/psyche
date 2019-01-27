#include "connection.h"
#include "channel.h"


void psyche::connection::send(std::string msg, sendCallback cb) {
	using namespace std::placeholders;
	send_callback_ = cb;
	write_buffer_->append(msg);
	soc_->write(*write_buffer_, std::bind(&connection::handleSend, this));
}

psyche::connection::connection(connection&& other) noexcept
	:soc_(std::move(other.soc_)),
	read_buffer_(std::move(other.read_buffer_)),
	write_buffer_(std::move(other.write_buffer_)),
	recv_callback_(other.recv_callback_),
	send_callback_(other.send_callback_)
{
	soc_->updateBuffer(*read_buffer_, *write_buffer_);
}
