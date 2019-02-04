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

	connection(context& c, int fd);
	connection(std::unique_ptr<socket>&& soc);
	connection(const connection&) = delete;
	connection(connection&& other) noexcept;

	void send(std::string msg, sendCallback cb);
	~connection();

	auto getReadCallback() const { return recv_callback_; }
	auto getWriteCallback() const { return send_callback_; }
	auto getCloseCallback() const { return close_callback_; }
	auto getErrorCallback() const { return error_callback_; }

	void setReadCallback(recvCallback cb);
	void setWriteCallback(sendCallback cb);
	void setCloseCallback(closeCallback cb);

	endpoint local_endpoint()const { return soc_->local_endpoint(); }
	endpoint peer_endpoint() const { return soc_->peer_endpoint(); }

private:
	void handleRecv();
	void handleSend();
	void handleClose();;

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
	connection_wrapper(connection_ptr c);
	connection_wrapper(const connection_wrapper& c);
	connection_wrapper(connection_wrapper&& c) noexcept;

	void send(std::string msg) const;

	endpoint local_endpoint()const { return conn->local_endpoint(); }
	endpoint peer_endpoint() const { return conn->peer_endpoint(); }

private:
	connection_ptr conn;
};

using Connection = connection_wrapper;
}
