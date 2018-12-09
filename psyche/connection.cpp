#include "connection.h"

void psyche::connection::receive(recvCallback cb) {
	using namespace std::placeholders;
	recv_callback_ = cb;
	soc_.read(read_buffer_, std::bind(&connection::handleRecv, this, _1, _2));
}

void psyche::connection::send(std::string msg, sendCallback cb) {
	using namespace std::placeholders;
	send_callback_ = cb;
	write_buffer_.append(msg);
	soc_.write(write_buffer_, std::bind(&connection::handleSend, this, _1, _2));
}

psyche::connection::connection(connection&& other) noexcept
	:context_(other.context_), soc_(std::move(other.soc_)), read_buffer_(std::move(read_buffer_)),
	write_buffer_(std::move(other.write_buffer_)),recv_callback_(other.recv_callback_),
	send_callback_(other.send_callback_)
{
	other.soc_.reset();
}
