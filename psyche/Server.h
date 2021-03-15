#pragma once
#include <set>
#include "connection.h"
#include "acceptor.h"
#include "thread_pool.h"

namespace psyche
{
class Server
{
public:
    using NewConnCallback = std::function<void(Connection)>;
    using ReadCallback = std::function<void(Connection,Buffer)>;
    using WriteCallback = std::function<void(Connection)>;
    using CloseCallback = std::function<void(Connection)>;
    using ErrorCallback = std::function<void(Connection,ErrorCode)>;

    Server(std::uint16_t port, const std::string& ip = "0.0.0.0");

    void set_new_conn_callback(NewConnCallback cb);
    void set_read_callback(ReadCallback cb);
    void set_write_callback(WriteCallback cb);
    void set_close_callback(CloseCallback cb);
    void set_error_callback(ErrorCallback cb);

    void start();
    void stop();
    void erase(ConnectionPtr con);

private:

    void handle_read(Connection con, Buffer buffer) const;
    void handle_write(Connection con) const;
    void handle_close(Connection con);

    context context_;
    Acceptor acceptor_;
    std::set<ConnectionPtr> connections_;

    NewConnCallback new_conn_callback_;
    ReadCallback read_callback_;
    WriteCallback write_callback_;
    CloseCallback close_callback_;
    ErrorCallback error_callback_;
};
}
