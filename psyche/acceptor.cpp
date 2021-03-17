#include "acceptor.h"

using namespace psyche;

Acceptor::Acceptor(Context& context, const Endpoint ep) : soc_(&context) {
    soc_.bind(ep);
}

void Acceptor::accept(const AcceptCallback& cb, const int backlog) {
    soc_.listen(backlog);
    soc_.get_context()->set_read_callback(soc_.fd(), [this]() { this->handle_new_connection(); }
    );
    accept_cb_ = cb;
    soc_.get_context()->get_channel(soc_.fd())->enable_reading();
}

void Acceptor::handle_new_connection() const {
    //if (ec) throw;
    auto new_soc = soc_.accept();
    if (accept_cb_) {
        accept_cb_(std::make_unique<ConnectionImpl>(std::move(new_soc)));
    }
}
