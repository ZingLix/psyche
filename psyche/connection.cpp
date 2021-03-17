#include "connection.h"
#include "channel.h"
#include "server.h"
using namespace std::placeholders;

void psyche::ConnectionImpl::send(const std::string& msg, SendCallback cb) {
    using namespace std::placeholders;
    send_callback_ = std::move(cb);
    {
        std::lock_guard<std::mutex> guard(w_buf_mutex_);
        write_buffer_->append(msg);
    }
    soc_->write(*write_buffer_, [this]() { this->handle_send(); });
}

psyche::ConnectionImpl::~ConnectionImpl() = default;

auto psyche::ConnectionImpl::get_read_callback() const { return recv_callback_; }

auto psyche::ConnectionImpl::get_write_callback() const { return send_callback_; }

auto psyche::ConnectionImpl::get_close_callback() const { return close_callback_; }

auto psyche::ConnectionImpl::get_error_callback() const { return error_callback_; }

void psyche::ConnectionImpl::set_read_callback(RecvCallback cb) {
    soc_->read(*read_buffer_, [this]() { this->handle_recv(); });
    recv_callback_ = std::move(cb);
}

void psyche::ConnectionImpl::set_write_callback(SendCallback cb) {
    //	soc_->write(*write_buffer_, std::bind(&ConnectionImpl::handle_send, this));
    send_callback_ = std::move(cb);
}

void psyche::ConnectionImpl::set_close_callback(CloseCallback cb) {
    soc_->set_close_callback([this]() { this->handle_close(); });
    close_callback_ = std::move(cb);
}

psyche::Endpoint psyche::ConnectionImpl::peer_endpoint() const { return peer_endpoint_; }

void psyche::ConnectionImpl::invoke_read_callback() {
    if (recv_callback_)
        recv_callback_(ConnectionWrapper(shared_from_this()), BufferWrapper(*read_buffer_));
}

void psyche::ConnectionImpl::invoke_send_callback() {
    if (send_callback_)
        send_callback_(ConnectionWrapper(shared_from_this()));
}

void psyche::ConnectionImpl::invoke_close_callback() {
    if (close_callback_)
        close_callback_(ConnectionWrapper(shared_from_this()));
}

void psyche::ConnectionImpl::handle_recv() {
    std::lock_guard<std::mutex> guard(r_buf_mutex_);
    const auto n = read_buffer_->read_fd(soc_->fd());
    if (n == 0) {
        handle_close();
        return;
    }
    invoke_read_callback();
}

void psyche::ConnectionImpl::handle_send() {
    //if(ec&&ec != 4) {
    //	if (send_callback_) send_callback_();
    //	else throw;
    //}
    {
        std::lock_guard<std::mutex> guard(w_buf_mutex_);
        write_buffer_->write_fd(soc_->fd());
        if (write_buffer_->current_size() == 0) soc_->disable_write();
    }
    invoke_send_callback();
    if (status_ == TOBECLOSED && write_buffer_->current_size() == 0) shutdown();
}

void psyche::ConnectionImpl::handle_close() {
    if (status_ != CLOSED) {
        status_ = CLOSED;
        invoke_close_callback();
    }
}

psyche::ConnectionWrapper::ConnectionWrapper(ConnectionPtr c): conn(std::move(c)) {
}

psyche::ConnectionWrapper::ConnectionWrapper(ConnectionWrapper&& c) noexcept
    : conn(std::move(c.conn)) {
}


void psyche::ConnectionWrapper::send(const std::string& msg) const {
    conn->send(msg, conn->get_write_callback());
}

psyche::ConnectionPtr psyche::ConnectionWrapper::pointer() const {
    return conn;
}

void psyche::ConnectionImpl::shutdown() {
    soc_->shutdown(SHUT_WR);
    status_ = CLOSING;
}


psyche::ConnectionImpl::ConnectionImpl(Context& c, int fd)
    : soc_(std::make_unique<Socket>(&c, fd)),
      read_buffer_(std::make_unique<BufferImpl>()),
      write_buffer_(std::make_unique<BufferImpl>()),
      status_(CONNECTED),
      local_endpoint_(soc_->local_endpoint()),
      peer_endpoint_(soc_->peer_endpoint()) {
}

psyche::ConnectionImpl::ConnectionImpl(std::unique_ptr<Socket>&& soc)
    : soc_(std::move(soc)),
      read_buffer_(std::make_unique<BufferImpl>()),
      write_buffer_(std::make_unique<BufferImpl>()),
      status_(CONNECTED),
      local_endpoint_(soc_->local_endpoint()),
      peer_endpoint_(soc_->peer_endpoint()) {
}

void psyche::ConnectionWrapper::set_read_callback(RecvCallback cb) const {
    conn->set_read_callback(std::move(cb));
}

void psyche::ConnectionWrapper::set_write_callback(SendCallback cb) const {
    conn->set_write_callback(std::move(cb));
}

void psyche::ConnectionWrapper::set_close_callback(CloseCallback cb) const {
    conn->set_close_callback(std::move(cb));
}

void psyche::ConnectionImpl::close() {
    if (write_buffer_->current_size() == 0) {
        shutdown();
        status_ = CLOSED;
    }
    else {
        status_ = TOBECLOSED;
    }
}

psyche::Endpoint psyche::ConnectionImpl::local_endpoint() const {
    return local_endpoint_;
}

void psyche::ConnectionWrapper::close() const {
    conn->close();
}

bool psyche::ConnectionWrapper::operator<(const ConnectionWrapper& other) const {
    return conn < other.conn;
}

psyche::ConnectionImpl::ConnectionImpl(ConnectionImpl&& other) noexcept
    : soc_(std::move(other.soc_)),
      read_buffer_(std::move(other.read_buffer_)),
      write_buffer_(std::move(other.write_buffer_)),
      status_(other.status_),
      local_endpoint_(other.local_endpoint_),
      peer_endpoint_(other.peer_endpoint_),
      recv_callback_(other.recv_callback_),
      send_callback_(other.send_callback_) {
}

psyche::ConnectionWrapper& psyche::ConnectionWrapper::operator=(const ConnectionWrapper& other) {
    conn = other.conn;
    return *this;
}

psyche::ConnectionWrapper& psyche::ConnectionWrapper::operator=(ConnectionWrapper&& other) noexcept {
    conn = std::move(other.conn);
    return *this;
}
