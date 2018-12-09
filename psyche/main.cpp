#include "context.h"
#include "socket.h"
#include "util.h"
#include "acceptor.h"
#include "connection.h"
#include <iostream>

int main() {
	psyche::context context;
	psyche::acceptor acceptor(context, psyche::endpoint("0.0.0.0",9981));
	std::vector<psyche::connection> connection_list;
//	connection_list.reserve(100);
	acceptor.accept([&](psyche::socket&& soc)  
	{
		connection_list.emplace_back(std::move(soc));
		auto& con = *(connection_list.end() - 1);
		con.receive([&](psyche::error_code,const char* str,std::size_t bytes)
		{
			std::string s(str, bytes);
			con.send(s,[=](psyche::error_code)
			{
				std::cout << "echo :" << s << std::endl;
			});
		});
	});
	context.run();
}
