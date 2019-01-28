#include "Server.h"

psyche::Server::Server(std::uint16_t port, const std::string& ip): acceptor_(context_, endpoint(ip, port)) {
	acceptor_.accept([&](std::unique_ptr<connection>&& conn)
	{
		connections_.emplace_back(std::move(conn));
		auto& c = connections_.back();
		using namespace std::placeholders;
		c->setReadCallback(std::bind(&Server::handleRead, this, _1, _2));
		c->setWriteCallback(std::bind(&Server::handleWrite, this, _1));
		c->setCloseCallback(std::bind(&Server::HandleClose, this, _1));
		if (new_conn_callback_) new_conn_callback_(c);
	});
}

void psyche::Server::setReadCallback(ReadCallback cb) {
	read_callback_ = cb;
}

void psyche::Server::setWriteCallback(WriteCallback cb) {
	write_callback_ = cb;
}

void psyche::Server::setCloseCallback(CloseCallback cb) {
	close_callback_ = cb;
}

void psyche::Server::setErrorCallback(ErrorCallback cb) {
	error_callback_ = cb;
}

void psyche::Server::start() {
	context_.run();
}

void psyche::Server::handleRead(connection_ptr con, buffer_impl& buffer) const {
	if (read_callback_) read_callback_(connection_wrapper(con), buffer_wrapper(buffer));
}

void psyche::Server::handleWrite(connection_ptr con) const {
	if (write_callback_) write_callback_(connection_wrapper(con));
}

void psyche::Server::HandleClose(connection_ptr con) {
	if (close_callback_) close_callback_(connection_wrapper(con));
	connections_.erase(std::find(connections_.begin(), connections_.end(), con));
}
