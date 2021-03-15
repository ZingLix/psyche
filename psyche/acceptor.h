#pragma once
#include "Socket.h"
#include "Channel.h"
#include <functional>
#include "connection.h"
#include <memory>

namespace psyche {
class Acceptor
{
public:
    using AcceptCallback = std::function<void(std::unique_ptr<ConnectionImpl>&&)>;

    Acceptor(Context& context,const Endpoint ep):soc_(&context) {
        soc_.bind(ep);
    }

    void accept(const AcceptCallback cb,const int backlog=1000) {
        soc_.listen(backlog);
        using namespace std::placeholders;
        soc_.get_context()->set_read_callback(soc_.fd(),
                                              std::bind(&Acceptor::handle_new_connection,this));
        accept_cb_ = cb;
        soc_.get_context()->get_channel(soc_.fd())->enable_reading();
    }

private:
    void handle_new_connection() const {
        //if (ec) throw;
        auto new_soc= soc_.accept();
        if(accept_cb_) {
            accept_cb_(std::make_unique<ConnectionImpl>(std::move(new_soc)));
        }
    }

    Socket soc_;
    AcceptCallback accept_cb_;
};
}
