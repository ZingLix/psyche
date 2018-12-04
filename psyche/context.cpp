#include "context.h"
#include "channel.h"

using namespace psyche;

context::context()
	:running_(false),quit_(false)
{
	
}


void context::set_revent(int fd, int events) {
	channel_map_[fd]->set_revents(events);
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
			channel_map_[it]->handleEvent();
		}
	}
	running_ = false;
}

void context::stop() {
	quit_ = true;
}

void context::set_read_callback(int fd, EventCallback& cb,  psyche::buffer_basic& buffer) {
	channel_map_[fd]->setReadCallback(cb, buffer);
}

void context::set_write_callback(int fd, EventCallback& cb,  psyche::buffer_basic& buffer) {
	channel_map_[fd]->setWriteCallback(cb, buffer);
}

void context::set_error_callback(int fd, EventCallback& cb,  psyche::buffer_basic& buffer) {
	channel_map_[fd]->setErrorCallback(cb, buffer);
}

