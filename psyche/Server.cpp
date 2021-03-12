#include "server.h"
#include <csignal>
#include <cassert>

psyche::Server::Server(std::uint16_t port, const std::string& ip): acceptor_(context_, endpoint(ip, port)) {
    acceptor_.accept([&](std::unique_ptr<connection>&& conn)
	{
		auto res = connections_.insert(std::make_shared<connection>(std::move(*conn)));
		//auto res= connections_.emplace(std::move(conn)).first;
		//connections_.emplace(std::move(conn));
		auto& c = *res.first;
		using namespace std::placeholders;
		c->setReadCallback(std::bind(&Server::handleRead, this, _1, _2));
		c->setWriteCallback(std::bind(&Server::handleWrite, this, _1));
		c->setCloseCallback(std::bind(&Server::handleClose, this, _1));
		if (new_conn_callback_) new_conn_callback_(c);
	});
}

void psyche::Server::setNewConnCallback(NewConnCallback cb) {
	new_conn_callback_ = cb;
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

void psyche::Server::stop() {
    context_.stop();
}

void psyche::Server::handleRead(Connection con, Buffer buffer) const {
	if (read_callback_) read_callback_(con, buffer);
}

void psyche::Server::handleWrite(Connection con) const {
	if (write_callback_) write_callback_(con);
}

void psyche::Server::handleClose(Connection con) {
	if (close_callback_) close_callback_(con);
    erase(con.pointer());
}

void psyche::Server::erase(connection_ptr con) {
    auto it = connections_.find(con);
    auto weak(it);
    if (it != connections_.end()) {
        connections_.erase(it);
    }
}
