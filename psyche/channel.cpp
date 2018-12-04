#include "channel.h"

#include "sys/epoll.h"

using namespace psyche;

const int psyche::channel::kNoneEvent = 0;
const int channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int channel::kWriteEvent = EPOLLOUT;

void channel::handleEvent() {
	if (revents_&(EPOLLERR)) {
		if (errorCallback_) error_cb();
	}
	if (revents_&(EPOLLIN | EPOLLPRI)) {
		if (readCallback_) read_cb();
	}
	if (revents_&EPOLLOUT) {
		if (writeCallback_) write_cb();
	}
}

void channel::error_cb() {
	if (errorCallback_) errorCallback_(errno,0);
}

void channel::read_cb() {
	auto length = buffer_->read_fd(fd_);
	if (readCallback_) readCallback_(errno, length);
}

void channel::write_cb() {
	auto length = buffer_->write_fd(fd_);
	if (writeCallback_) writeCallback_(errno, length);
}
