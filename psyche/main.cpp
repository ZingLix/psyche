#include "context.h"
#include "socket.h"
#include "util.h"

int main() {
	psyche::context context;
	psyche::socket socket;
	socket.bind(psyche::endpoint("127.0.0.1", 9999));
	auto s = socket.accept();
	std::vector<char> buffer(1024);
	s.read(psyche::buffer(buffer),[&](psyche::error_code,std::size_t length)
	{
		s.write(psyche::buffer(buffer), nullptr);
	});
	context.run();
}
