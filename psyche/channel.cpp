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
	std::size_t n = 0;
	if (read_buffer_ != nullptr) {
		n = read_buffer_->readFd(fd_);
		if(n==0) {
			if (closeCallback_) closeCallback_(errno, 0);
			return;
		}
	}
	if (readCallback_) readCallback_(errno, n);
}

void channel::write_cb() {
	std::size_t n = 0;
	if (write_buffer_ != nullptr) n = write_buffer_->writeFd(fd_);
	if (writeCallback_) writeCallback_(errno, n);
	disableWriting();
}
