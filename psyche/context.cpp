#include "context.h"
#include "channel.h"
#include <cassert>
using namespace psyche;

Context::Context()
    :running_(false),quit_(false),epoller_(std::make_unique<Epoller>(this))
{
}


void Context::set_revent(int fd, int events) {
    auto it = channel_map_.find(fd);
    if(it != channel_map_.end())
    it->second->set_revents(events);
    //channel_map_[fd].set_revents(events);
}

void Context::update_poller(int fd, int events) const {
    epoller_->update(fd, events);
}

void Context::run() {
    running_ = true;
    quit_ = false;
    while (!quit_) {
        fd_list_.clear();
        epoller_->poll(fd_list_);
        for(auto fd:fd_list_) {
            auto it = channel_map_.find(fd);
            if(it!=channel_map_.end())
                it->second->handle_event();
//			channel_map_[it].handle_event();
        }
    }
    running_ = false;
}

void Context::stop() {
    quit_ = true;
}

void Context::set_read_callback(int fd, EventCallback cb) {
    auto it = channel_map_.find(fd);
    assert(it != channel_map_.end());
    it->second->set_read_callback(cb);
//	channel_map_.find(fd)->second.set_read_callback(cb, buffer);
//	channel_map_[fd].set_read_callback(cb, buffer);
}

void Context::set_write_callback(int fd, EventCallback cb) {
    auto it = channel_map_.find(fd);
    assert(it != channel_map_.end());
    it->second->set_write_callback(cb);
//	channel_map_.find(fd)->second.set_write_callback(cb, buffer);
//	channel_map_[fd].set_write_callback(cb, buffer);
}

void Context::set_error_callback(int fd, EventCallback cb) {
    auto it = channel_map_.find(fd);
    assert(it != channel_map_.end());
    it->second->set_error_callback(cb);
    //	channel_map_.find(fd)->second.set_error_callback(cb);
//	channel_map_[fd].set_error_callback(cb);
}

Context::ChannelPtr Context::get_channel(int fd) {
    auto it = channel_map_.find(fd);
    if (it != channel_map_.end()) return (*it).second;
    return nullptr;
}

void Context::add_channel(int fd) {
    epoller_->add(fd);
    channel_map_.insert(std::make_pair(fd, std::make_shared<Channel>(this, fd)));
}

void Context::remove_channel(int fd) {
    epoller_->remove(fd);
    channel_map_.erase(channel_map_.find(fd));
}

