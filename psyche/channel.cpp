#include "channel.h"
#include "sys/epoll.h"

using namespace psyche;

const int channel::kNoneEvent = 0;
const int channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int channel::kWriteEvent = EPOLLOUT;

void channel::handleEvent() {
	auto t(context_->get_channel(fd()));
    if (t == nullptr) return;
	//if (revents_&(EPOLLERR)) {
	//	if (errorCallback_) error_cb();
	//	return;
	//}
	if(revents_&EPOLLRDHUP) {
		if (closeCallback_) close_cb();
		return;
	}
	if (revents_&(EPOLLIN | EPOLLPRI)) {
		if (readCallback_) read_cb();
	}
	if (revents_&EPOLLOUT) {
		if (writeCallback_) write_cb();
	}
}

void channel::set_revents(int revent) { revents_ = revent; }

int channel::events() const { return events_; }

int channel::fd() const { return fd_; }

bool channel::isNoneEvent() const { return events() == kNoneEvent; }

void channel::update() const {
	context_->update_poller(fd_, events());
}

channel::channel(context* c, int fd): context_(c), fd_(fd), events_(EPOLLERR|EPOLLRDHUP), revents_(0) {
	update();
}

void channel::setReadCallback(const EventCallback& cb) {
	readCallback_ = cb;
}

void channel::setWriteCallback(const EventCallback& cb) {
	writeCallback_ = cb;
}

void channel::setCloseCallback(const EventCallback& cb) {
	closeCallback_ = cb;
}

void channel::setErrorCallback(const EventCallback& cb) {
	errorCallback_ = cb;
}

void channel::enableReading() {
	events_ |= kReadEvent;
	update();
}

void channel::disableReading() {
	events_ &= ~kReadEvent;
	update();
}

void channel::enableWriting() {
	events_ |= kWriteEvent;
	update();
}

void channel::disableWriting() {
	events_ &= ~kWriteEvent;
	update();
}

void channel::error_cb() {
	if (errorCallback_) errorCallback_();
}

void channel::read_cb() {
	std::size_t n = 0;
	if (readCallback_) readCallback_();
}

void channel::write_cb() {
	if (fd_ == 0) return;
	if (writeCallback_) writeCallback_();
}

void channel::close_cb() {
	if (closeCallback_) closeCallback_();
}
