#include "context.h"
#include "socket.h"
#include "util.h"
#include "acceptor.h"
#include "connection.h"
#include <iostream>
#include <memory>

int main() {
	psyche::context context;
	psyche::acceptor acceptor(context, psyche::endpoint("0.0.0.0",9981));
	std::vector<std::shared_ptr<psyche::connection>> connection_list;
	acceptor.accept([&](std::unique_ptr<psyche::connection>&& conn)  
	{
		connection_list.emplace_back(std::move(conn));
		auto con = connection_list.back();
		con->receive([=](psyche::error_code,const char* str,std::size_t bytes)
		{
			std::string s(str, bytes);
			con->send(s,[=](psyche::error_code)
			{
				std::cout << "echo :" << s << std::endl;
			});
		});
	});
	context.run();
}
