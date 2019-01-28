#pragma once
#include <vector>
#include <algorithm>
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

	Server(std::uint16_t port,const std::string& ip = "0.0.0.0")
		:acceptor_(context_,endpoint(ip,port)) 
	{
		acceptor_.accept([&](std::unique_ptr<connection>&& conn)
		{
			connections_.emplace_back(std::move(conn));
			auto& c = connections_.back();
			using namespace std::placeholders;
			c->setReadCallback(std::bind(&Server::handleRead, this, _1, _2));
			c->setWriteCallback(std::bind(&Server::handleWrite, this, _1));
			c->setCloseCallback(std::bind(&Server::handleClose, this, _1));
			if (new_conn_callback_) new_conn_callback_(c);
		});
	}

 	void setReadCallback(ReadCallback cb) {
		read_callback_ = cb;
	}

	void setWriteCallback(WriteCallback cb) {
		write_callback_ = cb;
	}

	void setCloseCallback(CloseCallback cb) {
		close_callback_ = cb;
	}

	void setErrorCallback(ErrorCallback cb) {
		error_callback_ = cb;
	}

	void start() {
		context_.run();
	}

private:
	void handleRead(connection_ptr con, buffer_impl& buffer) {
		if (read_callback_) read_callback_(connection_wrapper(con), buffer_wrapper(buffer));
	}

	void handleWrite(connection_ptr con) {
		if (write_callback_) write_callback_(connection_wrapper(con));
	}

	void handleClose(connection_ptr con) {
		if (close_callback_) close_callback_(connection_wrapper(con));
		connections_.erase(std::find(connections_.begin(),connections_.end(),con));
	}
	context context_;
	acceptor acceptor_;
	std::vector<connection_ptr> connections_;

	NewConnCallback new_conn_callback_;
	ReadCallback read_callback_;
	WriteCallback write_callback_;
	CloseCallback close_callback_;
	ErrorCallback error_callback_;
};
}
