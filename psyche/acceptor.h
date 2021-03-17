#pragma once
#include "socket.h"
#include "channel.h"
#include <functional>
#include "connection.h"
#include <memory>

namespace psyche
{
class Acceptor
{
public:
    using AcceptCallback = std::function<void(std::unique_ptr<ConnectionImpl>&&)>;

    Acceptor(Context& context, Endpoint ep);

    void accept(const AcceptCallback& cb, int backlog = 1000);

private:
    void handle_new_connection() const;

    Socket soc_;
    AcceptCallback accept_cb_;
};
}
