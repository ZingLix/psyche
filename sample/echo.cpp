#include "context.h"
#include "socket.h"
#include "acceptor.h"
#include "connection.h"
#include <memory>
#include <algorithm>
#include "LogInfo.h"

int main() {
	psyche::context context;
	psyche::acceptor acceptor(context, psyche::endpoint("0.0.0.0",9981));
	std::vector<std::shared_ptr<psyche::connection>> connection_list;
	acceptor.accept([&](std::unique_ptr<psyche::connection>&& conn)  
	{
		connection_list.emplace_back(std::move(conn));
		auto con = connection_list.back();
		con->setRecvCallback([=](psyche::buffer& buffer)
		{
			con->send(buffer.retrieve(),nullptr);
		});
	});
	context.run();
}
