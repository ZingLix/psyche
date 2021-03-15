#pragma once
#include "socket.h"
#include <utility>
#include <memory>

namespace psyche {
class ConnectionImpl;
class ConnectionWrapper;
class Server;

using ConnectionPtr = std::shared_ptr<ConnectionImpl>;
using Connection = ConnectionWrapper;

using RecvCallback = std::function<void(Connection, Buffer)>;
using SendCallback = std::function<void(Connection)>;
using CloseCallback = std::function<void(Connection)>;
using ErrorCallback = std::function<void(Connection, ErrorCode)>;

class ConnectionImpl:public std::enable_shared_from_this<ConnectionImpl>
{
public:
    enum Status
    {
        EMPTY,
        CONNECTING,
        CONNECTED,
        TOBECLOSED,
        CLOSING,
        CLOSED
    };

    ConnectionImpl(context& c, int fd);
    ConnectionImpl(std::unique_ptr<socket>&& soc);
    ConnectionImpl(const ConnectionImpl&) = delete;
    ConnectionImpl(ConnectionImpl&& other) noexcept;

    void send(std::string msg, SendCallback cb);
    virtual ~ConnectionImpl();

    auto get_read_callback() const { return recv_callback_; }
    auto get_write_callback() const { return send_callback_; }
    auto get_close_callback() const { return close_callback_; }
    auto get_error_callback() const { return error_callback_; }

    void set_read_callback(RecvCallback cb);
    void set_write_callback(SendCallback cb);
    void set_close_callback(CloseCallback cb);

    void close();

    endpoint local_endpoint() const;
    endpoint peer_endpoint() const;

protected:
    virtual void invoke_read_callback();
    virtual void invoke_send_callback();
    virtual void invoke_close_callback();

    void handle_recv();
    void handle_send();
    void handle_close();

    void shutdown();

    std::unique_ptr<socket> soc_;
    std::mutex r_buf_mutex_, w_buf_mutex_;
    std::unique_ptr<BufferImpl> read_buffer_;
    std::unique_ptr<BufferImpl> write_buffer_;
    Status status_;
    endpoint local_endpoint_;
    endpoint peer_endpoint_;
    RecvCallback recv_callback_;
    SendCallback send_callback_;
    CloseCallback close_callback_;
    ErrorCallback error_callback_;
};

class ConnectionWrapper
{
public:
    ConnectionWrapper(ConnectionPtr c);
    ConnectionWrapper(const ConnectionWrapper& c);
    ConnectionWrapper(ConnectionWrapper&& c) noexcept;

    ConnectionWrapper& operator=(const ConnectionWrapper& other);
    ConnectionWrapper& operator=(ConnectionWrapper&& other) noexcept;

    void send(std::string msg) const;
    ConnectionPtr pointer() const;

    void setReadCallback(RecvCallback cb) const;
    void setWriteCallback(SendCallback cb) const;
    void setCloseCallback(CloseCallback cb) const;

    void close() const;

    endpoint local_endpoint()const { return conn->local_endpoint(); }
    endpoint peer_endpoint() const { return conn->peer_endpoint(); }

    bool operator<(const ConnectionWrapper& other) const;

private:
    ConnectionPtr conn;
};
}
