#pragma once
#include <set>
#include "connection.h"
#include "acceptor.h"

namespace psyche
{
class Server
{
public:
	using NewConnCallback = std::function<void(Connection)>;
	using ReadCallback = std::function<void(Connection,Buffer)>;
	using WriteCallback = std::function<void(Connection)>;
	using CloseCallback = std::function<void(Connection)>;
	using ErrorCallback = std::function<void(Connection,error_code)>;

	Server(std::uint16_t port, const std::string& ip = "0.0.0.0");

	void setNewConnCallback(NewConnCallback cb);
	void setReadCallback(ReadCallback cb);
	void setWriteCallback(WriteCallback cb);
	void setCloseCallback(CloseCallback cb);
	void setErrorCallback(ErrorCallback cb);

	void start();

private:
	void handleRead(connection_ptr con, buffer_impl& buffer) const;
	void handleWrite(connection_ptr con) const;
	void HandleClose(connection_ptr con);

	context context_;
	acceptor acceptor_;
	std::set<connection_ptr> connections_;

	NewConnCallback new_conn_callback_;
	ReadCallback read_callback_;
	WriteCallback write_callback_;
	CloseCallback close_callback_;
	ErrorCallback error_callback_;
};
}
