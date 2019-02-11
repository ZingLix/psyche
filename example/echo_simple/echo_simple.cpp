#include "psyche/psyche.h"
using namespace psyche;

int main() {
	Server s(9981);
	s.setReadCallback([](Connection con, Buffer buffer)
		{
			con.send(buffer.retrieveAll());
		});
	s.start();
}