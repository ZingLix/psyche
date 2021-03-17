#pragma once
#include "socket.h"
#include <memory>
#include <mutex>

namespace psyche
{
class ConnectionImpl;
class ConnectionWrapper;
class Server;

using ConnectionPtr = std::shared_ptr<ConnectionImpl>;
using Connection = ConnectionWrapper;

using RecvCallback = std::function<void(Connection, Buffer)>;
using SendCallback = std::function<void(Connection)>;
using CloseCallback = std::function<void(Connection)>;
using ErrorCallback = std::function<void(Connection, ErrorCode)>;

class ConnectionImpl : public std::enable_shared_from_this<ConnectionImpl>
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

    ConnectionImpl(Context& c, int fd);
    ConnectionImpl(std::unique_ptr<Socket>&& soc);
    ConnectionImpl(const ConnectionImpl&) = delete;
    ConnectionImpl(ConnectionImpl&& other) noexcept;

    void send(const std::string& msg, SendCallback cb);
    virtual ~ConnectionImpl();

    auto get_read_callback() const;
    auto get_write_callback() const;
    auto get_close_callback() const;
    auto get_error_callback() const;

    void set_read_callback(RecvCallback cb);
    void set_write_callback(SendCallback cb);
    void set_close_callback(CloseCallback cb);

    void close();

    Endpoint local_endpoint() const;
    Endpoint peer_endpoint() const;

protected:
    virtual void invoke_read_callback();
    virtual void invoke_send_callback();
    virtual void invoke_close_callback();

    void handle_recv();
    void handle_send();
    void handle_close();

    void shutdown();

    std::unique_ptr<Socket> soc_;
    std::mutex r_buf_mutex_, w_buf_mutex_;
    std::unique_ptr<BufferImpl> read_buffer_;
    std::unique_ptr<BufferImpl> write_buffer_;
    Status status_;
    Endpoint local_endpoint_;
    Endpoint peer_endpoint_;
    RecvCallback recv_callback_;
    SendCallback send_callback_;
    CloseCallback close_callback_;
    ErrorCallback error_callback_;
};

class ConnectionWrapper
{
public:
    ConnectionWrapper(ConnectionPtr c);
    ConnectionWrapper(const ConnectionWrapper& c) = default;
    ConnectionWrapper(ConnectionWrapper&& c) noexcept;

    ConnectionWrapper& operator=(const ConnectionWrapper& other);
    ConnectionWrapper& operator=(ConnectionWrapper&& other) noexcept;

    void send(const std::string& msg) const;
    ConnectionPtr pointer() const;

    void set_read_callback(RecvCallback cb) const;
    void set_write_callback(SendCallback cb) const;
    void set_close_callback(CloseCallback cb) const;

    void close() const;

    [[nodiscard]]
    Endpoint local_endpoint() const { return conn->local_endpoint(); }

    [[nodiscard]]
    Endpoint peer_endpoint() const { return conn->peer_endpoint(); }

    bool operator<(const ConnectionWrapper& other) const;

private:
    ConnectionPtr conn;
};
}
