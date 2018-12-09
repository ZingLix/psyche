#pragma once
#include "socket.h"
#include <utility>

namespace psyche {
class connection
{
public:
	using recvCallback = std::function<void(error_code, const char*, std::size_t)>;
	using sendCallback = std::function<void(error_code)>;

	connection(context& c, int fd) :context_(&c), soc_(&c,fd){}
	connection(socket&& soc) :context_(soc.get_context()),soc_(std::move(soc)) {}
	connection(const connection&) = delete;
	connection(connection&& other) noexcept;
	void receive(recvCallback cb);
	void send(std::string msg, sendCallback cb);

private:
	void handleRecv(error_code ec,std::size_t bytesTransferred) {
		if (ec&&ec!=4) {
			if (recv_callback_) recv_callback_(ec, nullptr, 0);
			else throw;
		}
		if (recv_callback_) recv_callback_(ec, read_buffer_.start(), bytesTransferred);
		read_buffer_.retrieve(bytesTransferred);
	}

	void handleSend(error_code ec,std::size_t bytesTransferred) {
		if(ec&&ec != 4) {
			if (send_callback_) send_callback_(ec);
			else throw;
		}
		if (send_callback_) send_callback_(ec);
		write_buffer_.retrieve(bytesTransferred);
	}

	context* context_;
	socket soc_;
	buffer read_buffer_;
	buffer write_buffer_;
	recvCallback recv_callback_;
	sendCallback send_callback_;
};
}
