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

void channel::set_revents(int revent) { revents_ = revent; }

int channel::events() const { return events_; }

int channel::fd() const { return fd_; }

bool channel::isNoneEvent() const { return events() == kNoneEvent; }

void channel::update_buffer(buffer_impl* r_buffer, buffer_impl* w_buffer) {
	read_buffer_ = r_buffer;
	write_buffer_ = w_buffer;
}

void channel::update() const {
	context_->update_poller(fd_, events());
}

channel::channel(context* c, int fd): context_(c), fd_(fd), events_(0), revents_(0), read_buffer_(nullptr),
                                      write_buffer_(nullptr) {

}

void channel::setReadCallback(const EventCallback& cb, psyche::buffer_impl* buffer) {
	readCallback_ = cb;
	read_buffer_ = buffer;
}

void channel::setWriteCallback(const EventCallback& cb, psyche::buffer_impl* buffer) {
	writeCallback_ = cb;
	write_buffer_ = buffer;
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
	if (read_buffer_ != nullptr) {
		n = read_buffer_->readFd(fd_);
		if(n==0&&errno!=EWOULDBLOCK) {
			if (closeCallback_) closeCallback_();
			return;
		}
	}
	if (readCallback_) readCallback_();
}

void channel::write_cb() {
	std::size_t n = 0;
	if (write_buffer_ != nullptr) n = write_buffer_->writeFd(fd_);
	if (writeCallback_) writeCallback_();
	disableWriting();
}
