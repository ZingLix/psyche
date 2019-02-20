#include "connection.h"
#include "channel.h"
#include "Server.h"
using namespace std::placeholders;

void psyche::connection::send(std::string msg, sendCallback cb) {
    using namespace std::placeholders;
    send_callback_ = cb;
    write_buffer_->append(msg);
    soc_->write(*write_buffer_, std::bind(&connection::handleSend, this));
}

psyche::connection::~connection() {
}

void psyche::connection::setReadCallback(recvCallback cb) {
    soc_->read(*read_buffer_, std::bind(&connection::handleRecv, this));
    recv_callback_ = cb;
}

void psyche::connection::setWriteCallback(sendCallback cb) {
//	soc_->write(*write_buffer_, std::bind(&connection::handleSend, this));
    send_callback_ = cb;
}

void psyche::connection::setCloseCallback(closeCallback cb) {
    soc_->setCloseCallback(std::bind(&connection::handleClose, this));
    close_callback_ = cb;
}

psyche::endpoint psyche::connection::peer_endpoint() const { return peer_endpoint_; }

void psyche::connection::invokeReadCallback() {
    if (recv_callback_) 
        recv_callback_(connection_wrapper(shared_from_this()), buffer_wrapper(*read_buffer_));
}

void psyche::connection::invokeSendCallback() {
    if (send_callback_) 
        send_callback_(connection_wrapper(shared_from_this()));
}

void psyche::connection::invokeCloseCallback() {
    if (close_callback_) 
        close_callback_(connection_wrapper(shared_from_this()));
}

void psyche::connection::handleRecv() {
    /*if (ec&&ec!=4) {
        if (recv_callback_) recv_callback_(ec, nullptr, 0);
        else throw;
    }*/
    auto n = read_buffer_->readFd(soc_->fd());
    if(n==0) {
        handleClose();
        return;
    }
    invokeReadCallback();
}

void psyche::connection::handleSend() {
    //if(ec&&ec != 4) {
    //	if (send_callback_) send_callback_();
    //	else throw;
    //}
    write_buffer_->writeFd(soc_->fd());
    if (write_buffer_->curSize() == 0) soc_->disableWrite();
    invokeSendCallback();
    if (status_==TOBECLOSED && write_buffer_->curSize() == 0) shutdown();
}

void psyche::connection::handleClose() {
    status_ = CLOSED;
    invokeCloseCallback();
    soc_.release();
}

void psyche::connection_s::invokeReadCallback() {
    if (recv_callback_) {
        server_->execute([=]() {
            soc_->disableRead();
            recv_callback_(connection_wrapper(shared_from_this()), buffer_wrapper(*read_buffer_));
            soc_->enableRead();
        });
    }
}

void psyche::connection_s::invokeSendCallback() {
    if (send_callback_) {
        server_->execute(send_callback_, connection_wrapper(shared_from_this()));
    }
}

void psyche::connection_s::invokeCloseCallback() {
    if(close_callback_) {
        close_callback_(connection_wrapper(shared_from_this()));
    }
    server_->erase(shared_from_this());
}


psyche::connection_wrapper::connection_wrapper(connection_ptr c): conn(c) {
}

psyche::connection_wrapper::connection_wrapper(const connection_wrapper& c)
    :conn(c.conn)
{
}

psyche::connection_wrapper::connection_wrapper(connection_wrapper&& c) noexcept
    :conn(std::move(c.conn))
{
}


void psyche::connection_wrapper::send(std::string msg) const {
    conn->send(msg, conn->getWriteCallback());
}

psyche::connection_ptr psyche::connection_wrapper::pointer() const {
    return conn;
}

void psyche::connection::shutdown() {
    soc_->shutdown(SHUT_WR);
    status_ = CLOSED;
}


psyche::connection::connection(context& c, int fd)
    : soc_(std::make_unique<socket>(&c, fd)),
    read_buffer_(std::make_unique<buffer_impl>()),
    write_buffer_(std::make_unique<buffer_impl>()),
    status_(CONNECTED),
    local_endpoint_(soc_->local_endpoint()),
    peer_endpoint_(soc_->peer_endpoint())
{
}

psyche::connection::connection(std::unique_ptr<socket>&& soc)
    : soc_(std::move(soc)),
    read_buffer_(std::make_unique<buffer_impl>()),
    write_buffer_(std::make_unique<buffer_impl>()),
    status_(CONNECTED),
    local_endpoint_(soc_->local_endpoint()),
    peer_endpoint_(soc_->peer_endpoint())
{
}

void psyche::connection_wrapper::setReadCallback(recvCallback cb) const {
    conn->setReadCallback(cb);
}

void psyche::connection_wrapper::setWriteCallback(sendCallback cb) const {
    conn->setWriteCallback(cb);
}

void psyche::connection_wrapper::setCloseCallback(closeCallback cb) const {
    conn->setCloseCallback(cb);
}

void psyche::connection::close() {
    if (write_buffer_->curSize() == 0) {
        shutdown();
        status_ = CLOSED;
    }else {
        status_ = TOBECLOSED;
    }
}

psyche::endpoint psyche::connection::local_endpoint() const {
    return local_endpoint_;
}

void psyche::connection_wrapper::close() const {
    conn->close();
}

bool psyche::connection_wrapper::operator<(const connection_wrapper& other) const {
    return conn < other.conn;
}

psyche::connection::connection(connection&& other) noexcept
    :soc_(std::move(other.soc_)),
    read_buffer_(std::move(other.read_buffer_)),
    write_buffer_(std::move(other.write_buffer_)),
    recv_callback_(other.recv_callback_),
    send_callback_(other.send_callback_),
    status_(other.status_),
    local_endpoint_(other.local_endpoint_),
    peer_endpoint_(other.peer_endpoint_)
{
}
