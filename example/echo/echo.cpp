#include "psyche/Server.h"
#include <iostream>
#include <signal.h>
using namespace psyche;

int main() {
	Server s(9981);
	s.setNewConnCallback([](Connection con)
	{
		LOG_INFO << "New connect from " << con.peer_endpoint().to_string() << ".";
	});
	s.setReadCallback([](Connection con,Buffer buffer)
	{
		auto msg(buffer.retrieveAll());
		LOG_INFO << "Received from (" << con.peer_endpoint().to_string()
			<< "):" << msg;
		con.send(msg);
		con.close();
	});
	s.setCloseCallback([](Connection con)
	{
		LOG_INFO << con.peer_endpoint().to_string() << " connection closed";
	});
	s.start();
}