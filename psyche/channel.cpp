#include "channel.h"
#include "sys/epoll.h"

using namespace psyche;

const int channel::kNoneEvent = 0;
const int channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int channel::kWriteEvent = EPOLLOUT;

void channel::handle_event() {
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

bool channel::is_none_event() const { return events() == kNoneEvent; }

void channel::update() const {
    context_->update_poller(fd_, events());
}

channel::channel(context* c, int fd): context_(c), fd_(fd), events_(EPOLLERR|EPOLLRDHUP), revents_(0) {
    update();
}

void channel::set_read_callback(const EventCallback& cb) {
    readCallback_ = cb;
}

void channel::set_write_callback(const EventCallback& cb) {
    writeCallback_ = cb;
}

void channel::set_close_callback(const EventCallback& cb) {
    closeCallback_ = cb;
}

void channel::set_error_callback(const EventCallback& cb) {
    errorCallback_ = cb;
}

void channel::enable_reading() {
    events_ |= kReadEvent;
    update();
}

void channel::disable_reading() {
    events_ &= ~kReadEvent;
    update();
}

void channel::enable_writing() {
    events_ |= kWriteEvent;
    update();
}

void channel::disable_writing() {
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
