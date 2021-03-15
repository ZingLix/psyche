#include "server.h"

psyche::Server::Server(std::uint16_t port, const std::string& ip): acceptor_(context_, Endpoint(ip, port)) {
    acceptor_.accept([&](std::unique_ptr<ConnectionImpl>&& conn)
    {
        auto res = connections_.insert(std::make_shared<ConnectionImpl>(std::move(*conn)));
        //auto res= connections_.emplace(std::move(conn)).first;
        //connections_.emplace(std::move(conn));
        auto& c = *res.first;
        using namespace std::placeholders;
        c->set_read_callback(std::bind(&Server::handle_read, this, _1, _2));
        c->set_write_callback(std::bind(&Server::handle_write, this, _1));
        c->set_close_callback(std::bind(&Server::handle_close, this, _1));
        if (new_conn_callback_) new_conn_callback_(c);
    });
}

void psyche::Server::set_new_conn_callback(NewConnCallback cb) {
    new_conn_callback_ = std::move(cb);
}

void psyche::Server::set_read_callback(ReadCallback cb) {
    read_callback_ = std::move(cb);
}

void psyche::Server::set_write_callback(WriteCallback cb) {
    write_callback_ = std::move(cb);
}

void psyche::Server::set_close_callback(CloseCallback cb) {
    close_callback_ = std::move(cb);
}

void psyche::Server::set_error_callback(ErrorCallback cb) {
    error_callback_ = std::move(cb);
}

void psyche::Server::start() {
    context_.run();
}

void psyche::Server::stop() {
    context_.stop();
}

void psyche::Server::handle_read(Connection con, Buffer buffer) const {
    if (read_callback_) read_callback_(con, buffer);
}

void psyche::Server::handle_write(Connection con) const {
    if (write_callback_) write_callback_(std::move(con));
}

void psyche::Server::handle_close(Connection con) {
    if (close_callback_) close_callback_(con);
    erase(con.pointer());
}

void psyche::Server::erase(ConnectionPtr con) {
    const auto it = connections_.find(con);
    if (it != connections_.end()) {
        connections_.erase(it);
    }
}
