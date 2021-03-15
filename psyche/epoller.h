#pragma once
#include <vector>
#include <sys/epoll.h>
#include <map>

namespace psyche
{
class Context;

class Epoller
{
public:
    static const int kEpollEvents = 100;

    Epoller(Context* c);
    ~Epoller();
    void add(int fd) const;
    void remove(int fd) const;
    void update(int fd, int events) const;
    void poll(std::vector<int>& channel_list);

private:
    int epollfd_;
    std::array<epoll_event, kEpollEvents> events_;
    Context* context_;
};
}
