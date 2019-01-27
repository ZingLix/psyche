#pragma once
#include "socket.h"
#include <utility>
#include <memory>
#include "LogInfo.h"

namespace psyche {
class connection:public std::enable_shared_from_this<connection>
{
public:
	using recvCallback = std::function<void(buffer&)>;
	using sendCallback = std::function<void()>;
	using closeCallback = std::function<void()>;
	using errorCallback = std::function<void(error_code)>;

	connection(context& c, int fd) :soc_(std::make_unique<socket>(&c,fd)),read_buffer_(std::make_unique<buffer>()),write_buffer_(std::make_unique<buffer>()){}
	connection(std::unique_ptr<socket>&& soc) :soc_(std::move(soc)), read_buffer_(std::make_unique<buffer>()), write_buffer_(std::make_unique<buffer>()) {}
	connection(const connection&) = delete;
	connection(connection&& other) noexcept;
	void setReadCallback(recvCallback cb) {
		recv_callback_ = cb;
	}
	void send(std::string msg, sendCallback cb);
	~connection() {
		LOG_INFO << "connection " << soc_->fd() << " start to destroy.";
	}
	void setRecvCallback(recvCallback cb) {
		using namespace std::placeholders;
		soc_->read(*read_buffer_,std::bind(&connection::handleRecv,this));
		recv_callback_ = cb;
	}

	void setCloseCallback(closeCallback cb) {
		using namespace std::placeholders;
		soc_->setCloseCallback(std::bind(&connection::handleClose, this));
		close_callback_ = cb;
	}

private:
	void handleRecv() {
		/*if (ec&&ec!=4) {
			if (recv_callback_) recv_callback_(ec, nullptr, 0);
			else throw;
		}*/
		if (recv_callback_) recv_callback_(*read_buffer_);
	}

	void handleSend() {
		//if(ec&&ec != 4) {
		//	if (send_callback_) send_callback_();
		//	else throw;
		//}
		if (send_callback_) send_callback_();
	}

	void handleClose() {
		close_callback_();

		recv_callback_ = nullptr;
		send_callback_ = nullptr;
		close_callback_ = nullptr;
		error_callback_ = nullptr;
	};

	std::unique_ptr<socket> soc_;
	std::unique_ptr<buffer> read_buffer_;
	std::unique_ptr<buffer> write_buffer_;
	recvCallback recv_callback_;
	sendCallback send_callback_;
	closeCallback close_callback_;
	errorCallback error_callback_;
};
}
