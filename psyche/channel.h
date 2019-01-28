#pragma once
#include <functional>

#include "context.h"
#include "socket.h"
#include "util.h"

namespace psyche {
class channel
{
public:

	channel(context* c, int fd) :context_(c), fd_(fd),events_(0),revents_(0), read_buffer_(nullptr),write_buffer_(nullptr) {

	}

	void setReadCallback(const EventCallback& cb, psyche::buffer_impl* buffer) {
		readCallback_ = cb;
		read_buffer_ = buffer;
	}
	void setWriteCallback(const EventCallback& cb, psyche::buffer_impl* buffer) {
		writeCallback_ = cb;
		write_buffer_ = buffer;
	}
	void setCloseCallback(const EventCallback& cb) {
		closeCallback_ = cb;
	}
	void setErrorCallback(const EventCallback& cb) {
		errorCallback_ = cb;
	}

	void enableReading() {
		events_ |= kReadEvent; update();
	}
	void disableReading() {
		events_ &= ~kReadEvent; update();
	}
	void enableWriting() { events_ |= kWriteEvent; update(); }
	void disableWriting() { events_ &= ~kWriteEvent; update(); }

	void error_cb();
	void read_cb();
	void write_cb();

	void handleEvent();

	void set_revents(int revent) { revents_ = revent; }
	int events() const { return events_; }
	int fd() const { return fd_; }
	bool isNoneEvent()const { return events() == kNoneEvent; }
	void update_buffer(buffer_impl* r_buffer,buffer_impl* w_buffer) {
		read_buffer_ = r_buffer;
		write_buffer_ = w_buffer;
	}

private:
	void update() {
		context_->update_poller(fd_, events());
	}

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	context* context_;
	const int fd_;
	int events_;
	int revents_;

	buffer_impl* read_buffer_;
	buffer_impl* write_buffer_;

	EventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
};
}