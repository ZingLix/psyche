#pragma once
#include "socket.h"
#include "channel.h"
#include <functional>
#include "connection.h"
#include <memory>

namespace psyche {
class acceptor
{
public:
	using acceptCallback = std::function<void(std::unique_ptr<connection>&&)>;

	acceptor(context& context, endpoint ep):soc_(&context) {
		soc_.bind(ep);
	}

	void accept(acceptCallback cb,int backlog=1000) {
		soc_.listen(backlog);
		using namespace std::placeholders;
		soc_.get_context()->set_read_callback(soc_.fd(),
		                                      std::bind(&acceptor::handleNewConn,this,_1,_2),
		                                      nullptr);
		accept_cb_ = cb;
		soc_.get_context()->get_channel(soc_.fd())->enableReading();
	}

private:
	void handleNewConn(error_code ec, std::size_t size) {
		//if (ec) throw;
		auto new_soc= soc_.accept();
		if(accept_cb_) {
			accept_cb_(std::make_unique<connection>(std::move(new_soc)));
		}
	}

	socket soc_;
	acceptCallback accept_cb_;
};
}
