#include "Epoller.h"
#include "Channel.h"
#include <cstring>

using namespace psyche;

Epoller::Epoller(Context* c)
    :epollfd_(epoll_create(kEpollEvents+1)),
    context_(c)
{
}

Epoller::~Epoller() {
    ::close(epollfd_);
}


void Epoller::add(int fd) {
    epoll_event ev;
    memset(&ev, 0, sizeof ev);
    ev.data.fd = fd;
    ev.events = 0;
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
}

void Epoller::remove(int fd) {
    epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr);
}

void Epoller::update(int fd, int events) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
}


void Epoller::poll(std::vector<int>& channel_list) {
    auto num = epoll_wait(epollfd_, &*events_.begin(), kEpollEvents, 2000);
    for(int i=0;i<num;i++) {
        int fd = events_[i].data.fd;
        channel_list.push_back(fd);
        context_->set_revent(fd, events_[i].events);
    }
}
