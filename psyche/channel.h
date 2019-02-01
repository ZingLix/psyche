#pragma once
#include <functional>

#include "context.h"
#include "socket.h"
#include "util.h"

namespace psyche {
class channel:std::enable_shared_from_this<channel>
{
public:

	channel(context* c, int fd);

	void setReadCallback(const EventCallback& cb);
	void setWriteCallback(const EventCallback& cb);
	void setCloseCallback(const EventCallback& cb);
	void setErrorCallback(const EventCallback& cb);

	void enableReading();
	void disableReading();
	void enableWriting();
	void disableWriting();

	void error_cb();
	void read_cb();
	void write_cb();
	void close_cb();
	void handleEvent();

	void set_revents(int revent);
	int events() const;
	int fd() const;
	bool isNoneEvent() const;

private:
	void update() const;

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	context* context_;
	const int fd_;
	int events_;
	int revents_;

	EventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
};
}