#pragma once
#include "socket.h"
#include <utility>
#include <memory>
#include "LogInfo.h"

namespace psyche {
class connection;

using connection_ptr = std::shared_ptr<connection>;

class connection:public std::enable_shared_from_this<connection>
{
public:
	using recvCallback = std::function<void(connection_ptr,buffer_impl&)>;
	using sendCallback = std::function<void(connection_ptr)>;
	using closeCallback = std::function<void(connection_ptr)>;
	using errorCallback = std::function<void(connection_ptr,error_code)>;

	connection(context& c, int fd) :soc_(std::make_unique<socket>(&c,fd)),read_buffer_(std::make_unique<buffer_impl>()),write_buffer_(std::make_unique<buffer_impl>()){}
	connection(std::unique_ptr<socket>&& soc) :soc_(std::move(soc)), read_buffer_(std::make_unique<buffer_impl>()), write_buffer_(std::make_unique<buffer_impl>()) {}
	connection(const connection&) = delete;
	connection(connection&& other) noexcept;

	void send(std::string msg, sendCallback cb);
	~connection() {
		LOG_INFO << "connection " << soc_->fd() << " start to destroy.";
	}
	
	void setReadCallback(recvCallback cb) {
		using namespace std::placeholders;
		soc_->read(*read_buffer_,std::bind(&connection::handleRecv,this));
		recv_callback_ = cb;
	}

	auto getReadCallback() const { return recv_callback_; }
	auto getWriteCallback() const { return send_callback_; }
	auto getCloseCallback() const { return close_callback_; }
	auto getErrorCallback() const { return error_callback_; }

	void setWriteCallback(sendCallback cb) {
		using namespace std::placeholders;
		soc_->write(*write_buffer_, std::bind(&connection::handleSend, this));
		send_callback_ = cb;
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
		if (recv_callback_) recv_callback_(shared_from_this(),*read_buffer_);
	}

	void handleSend() {
		//if(ec&&ec != 4) {
		//	if (send_callback_) send_callback_();
		//	else throw;
		//}
		if (send_callback_) send_callback_(shared_from_this());
	}

	void handleClose() {
		close_callback_(shared_from_this());

		recv_callback_ = nullptr;
		send_callback_ = nullptr;
		close_callback_ = nullptr;
		error_callback_ = nullptr;
	};

	std::unique_ptr<socket> soc_;
	std::unique_ptr<buffer_impl> read_buffer_;
	std::unique_ptr<buffer_impl> write_buffer_;
	recvCallback recv_callback_;
	sendCallback send_callback_;
	closeCallback close_callback_;
	errorCallback error_callback_;
};

class connection_wrapper
{
public:
	connection_wrapper(connection_ptr c) :conn(c){}

	void send(std::string msg) {
		conn->send(msg, conn->getWriteCallback());
	}

private:
	connection_ptr conn;
};
using Connection = connection_wrapper;
}
