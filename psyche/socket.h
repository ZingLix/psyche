#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include "endpoint.h"
#include <functional>
#include "context.h"
#include "buffer.h"
#include "util.h"

namespace psyche
{
class Socket
{
public:
    const int shutdown_rd = SHUT_RD;
    const int shutdown_wr = SHUT_WR;
    const int shutdown_both = SHUT_RDWR;

    using ReadCallback = EventCallback;
    using WriteCallback = EventCallback;
    using CloseCallback = EventCallback;

    Socket(Context* c);
    Socket(Context* c, int fd);
    Socket(const Socket&) = delete;
    Socket(Socket&& soc) noexcept = delete;
    ~Socket();
    void shutdown(int how) const;
    void bind(const Endpoint& ep) const;
    void connect(const Endpoint& ep) const;
    void listen(int backlog = 1000) const;
    [[nodiscard]]
    std::unique_ptr<Socket> accept() const;
    void close() const;

    [[nodiscard]]
    Endpoint local_endpoint() const;
    [[nodiscard]]
    Endpoint peer_endpoint() const;

    [[nodiscard]]
    Context* get_context() const { return context_; }

    [[nodiscard]]
    int fd() const { return fd_; }

    void read(BufferImpl& buffer, ReadCallback) const;
    void write(BufferImpl& buffer, WriteCallback) const;
    void handle_close() const;

    void set_close_callback(std::function<void()>);

    void enable_read() const;
    void disable_read() const;
    void disable_write() const;

    void reset();
private:
    static inline sockaddr* sockaddr_cast(sockaddr_in& sock_in);
    static inline sockaddr* sockaddr_cast(sockaddr_in* sock_in);

    int fd_;
    Context* context_;
    std::function<void()> cb_;
};
}
