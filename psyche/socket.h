#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include "endpoint.h"
#include <functional>
#include "context.h"
#include "buffer.h"
#include "util.h"

namespace psyche {

class socket
{
public:
    const int shutdown_rd = SHUT_RD;
    const int shutdown_wr = SHUT_WR;
    const int shutdown_both = SHUT_RDWR;

    using ReadCallback = EventCallback;
    using WriteCallback = EventCallback;
    using CloseCallback = EventCallback;

    socket(context * c);
    socket(context * c,int fd);
    socket(const socket&) = delete;
    socket(socket&& soc) noexcept = delete;
    ~socket();
    void shutdown(int how) const;
    void bind(const endpoint& ep) const;
    void connect(const endpoint& ep) const;
    void listen(int backlog=1000) const;
    std::unique_ptr<socket> accept() const;
    void close() const;

    endpoint local_endpoint() const;
    endpoint peer_endpoint() const;

    context* get_context() { return context_; }
    int fd() { return fd_; }

    void read(BufferImpl& buffer, ReadCallback);
    void write(BufferImpl& buffer, WriteCallback);
    void handle_close();

    void set_close_callback(std::function<void()>);

    void enable_read();
    void disable_read();
    void disable_write();

    void reset();
private:
    static inline sockaddr* sockaddr_cast(sockaddr_in& sock_in);
    static inline sockaddr* sockaddr_cast(sockaddr_in* sock_in);

    int fd_;
    context* context_;
    std::function<void()> cb;
};
}
