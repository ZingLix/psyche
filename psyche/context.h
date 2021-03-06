#pragma once
#include <unordered_map>
#include <memory>
#include "epoller.h"
#include "util.h"
#include "buffer.h"

namespace psyche
{
class Channel;

class Context
{
public:
    using ChannelPtr = std::shared_ptr<Channel>;

    Context();

    void run();
    void set_revent(int fd, int events);
    void update_poller(int fd, int events) const;
    void set_read_callback(int fd, EventCallback cb);
    void set_write_callback(int fd, EventCallback cb);
    void set_error_callback(int fd, EventCallback cb);
    ChannelPtr get_channel(int fd);
    void stop();

    void add_channel(int fd);
    void remove_channel(int fd);
private:

    bool running_;
    bool quit_;
    std::unordered_map<int, ChannelPtr> channel_map_;
    std::unique_ptr<Epoller> epoller_;
    std::vector<int> fd_list_;
};
}
