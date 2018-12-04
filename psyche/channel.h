#pragma once
#include <functional>

#include "context.h"
#include "socket.h"
#include "util.h"

namespace psyche {
class channel
{
public:

	channel(context* c, int fd) :context_(c), fd_(fd) {

	}

	void setReadCallback(const EventCallback& cb, psyche::buffer_basic* buffer) {
		readCallback_ = cb;
		buffer_ = buffer;
	}
	void setWriteCallback(const EventCallback& cb, psyche::buffer_basic* buffer) {
		writeCallback_ = cb;
		buffer_ = buffer;
	}
	void setCloseCallback(const EventCallback& cb, psyche::buffer_basic* buffer) {
		closeCallback_ = cb;
		buffer_ = buffer;
	}
	void setErrorCallback(const EventCallback& cb, psyche::buffer_basic* buffer) {
		errorCallback_ = cb;
		buffer_ = buffer;
	}

	void enableReading() { events_ |= kReadEvent; update(); }
	void disableReading() { events_ &= ~kReadEvent; update(); }
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

	psyche::buffer_basic* buffer_;

	EventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
};
}