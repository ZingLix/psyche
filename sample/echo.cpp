#include "Server.h"

int main() {
	psyche::Server s("0.0.0.0", 9981);
	s.setReadCallback([](psyche::connection_ptr con,psyche::buffer& buffer)
	{
		con->send(buffer.retrieve(),nullptr);
	});
	s.start();
}