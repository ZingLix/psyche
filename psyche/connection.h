#pragma once
#include "socket.h"
#include <utility>
#include <memory>
#include "LogInfo.h"

namespace psyche {
class connection;
class connection_wrapper;
class Server;

using connection_ptr = std::shared_ptr<connection>;
using Connection = connection_wrapper;

using recvCallback = std::function<void(Connection, Buffer)>;
using sendCallback = std::function<void(Connection)>;
using closeCallback = std::function<void(Connection)>;
using errorCallback = std::function<void(Connection, error_code)>;

class connection:public std::enable_shared_from_this<connection>
{
public:
	enum status
	{
		EMPTY,
		CONNECTING,
		CONNECTED,
		TOBECLOSED,
		CLOSED
	};

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

	void close();

	endpoint local_endpoint() const;
	endpoint peer_endpoint() const;

protected:
	void invokeReadCallback();
	void invokeSendCallback();
	void invokeCloseCallback();

	void handleRecv();
	void handleSend();
	void handleClose();

	void shutdown();

	std::unique_ptr<socket> soc_;
	std::unique_ptr<buffer_impl> read_buffer_;
	std::unique_ptr<buffer_impl> write_buffer_;
	status status_;
	endpoint local_endpoint_;
	endpoint peer_endpoint_;
	recvCallback recv_callback_;
	sendCallback send_callback_;
	closeCallback close_callback_;
	errorCallback error_callback_;
};

class connection_s:public connection
{
public:
	connection_s(context& c, int fd,Server& s):connection(c,fd),server_(&s){}
	connection_s(std::unique_ptr<socket>&& soc, Server& s):connection(std::move(soc)),server_(&s){}
	connection_s(const connection_s&) = delete;
	connection_s(connection_s&& other) noexcept :connection(std::move(other)),server_(other.server_){}
protected:
	void invokeReadCallback();
	void invokeSendCallback();
	void invokeCloseCallback();
	Server* server_;
};

class connection_wrapper
{
public:
	connection_wrapper(connection_ptr c);
	connection_wrapper(const connection_wrapper& c);
	connection_wrapper(connection_wrapper&& c) noexcept;

	void send(std::string msg) const;
	connection_ptr pointer() const;

	void setReadCallback(recvCallback cb) const;
	void setWriteCallback(sendCallback cb) const;
	void setCloseCallback(closeCallback cb) const;

	void close() const;

	endpoint local_endpoint()const { return conn->local_endpoint(); }
	endpoint peer_endpoint() const { return conn->peer_endpoint(); }

	bool operator<(const connection_wrapper& other) const;

private:
	connection_ptr conn;
};
}
