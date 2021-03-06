#pragma once
#include <functional>

#include "context.h"
#include "util.h"

namespace psyche
{
class Channel : std::enable_shared_from_this<Channel>
{
public:

    Channel(Context* c, int fd);

    void set_read_callback(const EventCallback& cb);
    void set_write_callback(const EventCallback& cb);
    void set_close_callback(const EventCallback& cb);
    void set_error_callback(const EventCallback& cb);

    void enable_reading();
    void disable_reading();
    void enable_writing();
    void disable_writing();

    void error_cb() const;
    void read_cb() const;
    void write_cb() const;
    void close_cb() const;
    void handle_event();

    void set_revents(int revent);
    int events() const;
    int fd() const;
    bool is_none_event() const;

private:
    void update() const;

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    Context* context_;
    const int fd_;
    int events_;
    int revents_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};
}
