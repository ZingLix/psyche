#pragma once
#include <functional>

#include "context.h"
#include "socket.h"
#include "util.h"

namespace psyche {
class channel
{
public:

	channel(context* c, int fd);

	void setReadCallback(const EventCallback& cb, psyche::buffer_impl* buffer);
	void setWriteCallback(const EventCallback& cb, psyche::buffer_impl* buffer);
	void setCloseCallback(const EventCallback& cb);
	void setErrorCallback(const EventCallback& cb);

	void enableReading();
	void disableReading();
	void enableWriting();
	void disableWriting();

	void error_cb();
	void read_cb();
	void write_cb();

	void handleEvent();

	void set_revents(int revent);
	int events() const;
	int fd() const;
	bool isNoneEvent() const;

	void update_buffer(buffer_impl* r_buffer, buffer_impl* w_buffer);

private:
	void update() const;

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