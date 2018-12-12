#pragma once
#include "socket.h"
#include <utility>
#include <memory>
#include "LogInfo.h"

namespace psyche {
class connection:public std::enable_shared_from_this<connection>
{
public:
	using recvCallback = std::function<void(error_code, const char*, std::size_t)>;
	using sendCallback = std::function<void(error_code)>;
	using closeCallback = std::function<void(error_code)>;
	
	connection(context& c, int fd) :context_(&c), soc_(std::make_unique<socket>(&c,fd)){}
	connection(std::unique_ptr<socket>&& soc) :context_(soc->get_context()),soc_(std::move(soc)) {}
	connection(const connection&) = delete;
	connection(connection&& other) noexcept;
	void receive(recvCallback cb);
	void send(std::string msg, sendCallback cb);
	~connection() {
		LOG_INFO << "connection " << soc_->fd() << " start to destroy.";
	}
	void setCloseCallback(closeCallback cb) {
		using namespace std::placeholders;
		soc_->setCloseCallback(std::bind(&connection::handleClose, this, _1));
		close_callback_ = cb;
	}

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

	void handleClose(error_code ec) {
		recv_callback_ = nullptr;
		send_callback_ = nullptr;
		close_callback_(ec);
		close_callback_ = nullptr;
	};

	context* context_;
	std::unique_ptr<socket> soc_;
	buffer read_buffer_;
	buffer write_buffer_;
	recvCallback recv_callback_;
	sendCallback send_callback_;
	closeCallback close_callback_;
};
}
