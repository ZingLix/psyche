#include "context.h"
#include "channel.h"
#include <cassert>
using namespace psyche;

context::context()
	:running_(false),quit_(false),epoller_(std::make_unique<epoller>(this))
{
}


void context::set_revent(int fd, int events) {
	auto it = channel_map_.find(fd);
	assert(it != channel_map_.end());
	it->second.set_revents(events);
	//channel_map_[fd].set_revents(events);
}

void context::update_poller(int fd, int events) {
	epoller_->update(fd, events);
}

void context::run() {
	running_ = true;
	quit_ = false;
	while (!quit_) {
		fd_list_.clear();
		epoller_->poll(fd_list_);
		for(auto it:fd_list_) {
			channel_map_.find(it)->second.handleEvent();
//			channel_map_[it].handleEvent();
		}
	}
	running_ = false;
}

void context::stop() {
	quit_ = true;
}

void context::set_read_callback(int fd, EventCallback cb, buffer_impl* buffer) {
	auto it = channel_map_.find(fd);
	assert(it != channel_map_.end());
	it->second.setReadCallback(cb, buffer);
//	channel_map_.find(fd)->second.setReadCallback(cb, buffer);
//	channel_map_[fd].setReadCallback(cb, buffer);
}

void context::set_write_callback(int fd, EventCallback cb, buffer_impl* buffer) {
	auto it = channel_map_.find(fd);
	assert(it != channel_map_.end());
	it->second.setWriteCallback(cb, buffer);
//	channel_map_.find(fd)->second.setWriteCallback(cb, buffer);
//	channel_map_[fd].setWriteCallback(cb, buffer);
}

void context::set_error_callback(int fd, EventCallback cb, buffer_impl* buffer) {
	auto it = channel_map_.find(fd);
	assert(it != channel_map_.end());
	it->second.setErrorCallback(cb);
	//	channel_map_.find(fd)->second.setErrorCallback(cb);
//	channel_map_[fd].setErrorCallback(cb);
}

channel* context::get_channel(int fd) {
	auto it = channel_map_.find(fd);
	if (it != channel_map_.end()) return &(*it).second;
	return nullptr;
}

void context::add_channel(int fd) {
	channel_map_.insert(std::make_pair(fd, channel(this, fd)));
	epoller_->add(fd);
}

void context::remove_channel(int fd) {
	epoller_->remove(fd);
	channel_map_.erase(channel_map_.find(fd));
}

