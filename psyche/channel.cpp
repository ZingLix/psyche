#include "channel.h"
#include "sys/epoll.h"

using namespace psyche;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

void Channel::handle_event() {
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

void Channel::set_revents(int revent) { revents_ = revent; }

int Channel::events() const { return events_; }

int Channel::fd() const { return fd_; }

bool Channel::is_none_event() const { return events() == kNoneEvent; }

void Channel::update() const {
    context_->update_poller(fd_, events());
}

Channel::Channel(Context* c, int fd): context_(c), fd_(fd), events_(EPOLLERR|EPOLLRDHUP), revents_(0) {
    update();
}

void Channel::set_read_callback(const EventCallback& cb) {
    readCallback_ = cb;
}

void Channel::set_write_callback(const EventCallback& cb) {
    writeCallback_ = cb;
}

void Channel::set_close_callback(const EventCallback& cb) {
    closeCallback_ = cb;
}

void Channel::set_error_callback(const EventCallback& cb) {
    errorCallback_ = cb;
}

void Channel::enable_reading() {
    events_ |= kReadEvent;
    update();
}

void Channel::disable_reading() {
    events_ &= ~kReadEvent;
    update();
}

void Channel::enable_writing() {
    events_ |= kWriteEvent;
    update();
}

void Channel::disable_writing() {
    events_ &= ~kWriteEvent;
    update();
}

void Channel::error_cb() {
    if (errorCallback_) errorCallback_();
}

void Channel::read_cb() {
    std::size_t n = 0;
    if (readCallback_) readCallback_();
}

void Channel::write_cb() {
    if (fd_ == 0) return;
    if (writeCallback_) writeCallback_();
}

void Channel::close_cb() {
    if (closeCallback_) closeCallback_();
}
