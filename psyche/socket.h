#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
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
	using closeCallback = EventCallback;

	socket(context * c);
	socket(context * c,int fd);
	socket(const socket&) = delete;
	socket(socket&& soc) noexcept = delete;
	~socket();
	void shutdown(int how) const;
	void bind(const endpoint& ep) const;
	void connect(const endpoint& ep) const;
	void listen(int backlog=1000) const;
	std::unique_ptr<socket> accept() const;
	void close() const;

	endpoint local_endpoint() const;
	endpoint peer_endpoint() const;

	context* get_context() { return context_; }
	int fd() { return fd_; }

	void read(buffer& buffer, readCallback);
	void write(buffer& buffer, writeCallback);
	void handleClose(error_code ec, std::size_t);

	void setCloseCallback(std::function<void(error_code)>);

	void reset();
private:
	static inline sockaddr* sockaddr_cast(sockaddr_in& sock_in);
	static inline sockaddr* sockaddr_cast(sockaddr_in* sock_in);

	int fd_;
	context* context_;
	std::function<void(error_code)> cb;
};
}
