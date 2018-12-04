#pragma once
#include <sys/socket.h>
#include "endpoint.h"
#include <functional>
#include "context.h"
#include "buffer.hpp"
#include "util.h"

namespace psyche {

class socket
{
public:
	const int shutdown_rd = SHUT_RD;
	const int shutdown_wr = SHUT_WR;
	const int shutdown_both = SHUT_RDWR;

	using readCallback = EventCallback;
	using writeCallback = EventCallback;

	socket();
	socket(int fd);
	void shutdown(int how) const;
	void bind(const endpoint& ep) const;
	void connect(const endpoint& ep) const;
	void listen(int backlog) const;
	socket accept() const;
	void close() const;

	endpoint local_endpoint() const;
	endpoint peer_endpoint() const;


	void read(const buffer_basic& buffer, readCallback);
	void write(const buffer_basic& buffer, writeCallback);

private:
	static inline sockaddr* sockaddr_cast(sockaddr_in& sock_in);
	static inline sockaddr* sockaddr_cast(sockaddr_in* sock_in);

	int fd_;
	context* context_;
};
}
