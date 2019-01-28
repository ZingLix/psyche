#pragma once
#include <string>
#include <netinet/in.h>
#include "address.h"

namespace psyche {

class endpoint
{
public:
	endpoint(const std::string& ip, std::uint16_t port);
	endpoint(const sockaddr_in& sockaddr);
	ip::address address() const;
	std::uint16_t port() const;
	sockaddr_in sockaddr() const;

private:
	ip::address address_;
	std::uint16_t port_;
};
}