#include "psyche/Server.h"
#include <iostream>
using namespace psyche;

int main() {
	Server s(9981);
	s.setNewConnCallback([](Connection con)
	{
		std::cout << "New connect from " << con.peer_endpoint().to_string() << "."<<std::endl;
	});
	s.setReadCallback([](Connection con,Buffer buffer)
	{
		auto msg(buffer.retrieveAll());
		std::cout << "Received from " << con.peer_endpoint().to_string()
			<< " :" << msg;
		con.send(msg);
	});
	s.setCloseCallback([](Connection con)
	{
		LOG_INFO << con.peer_endpoint().address().to_string() << ":"
			<< con.peer_endpoint().port() << " connection closed";
	});
	s.start();
}