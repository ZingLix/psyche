#include "psyche/Server.h"
using namespace psyche;

int main() {
	Server s(9981);
	s.setReadCallback([](Connection con, Buffer buffer)
		{
			con.send(buffer.retrieve());
		});
	s.start();
}