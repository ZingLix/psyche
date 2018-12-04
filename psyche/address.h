#pragma once
#include <arpa/inet.h>
#include <string>

namespace psyche {
namespace ip
{

using port = std::uint16_t;

class address
{
public:
	address(const char* addr);
	address(const std::string addr);
	address(uint32_t addr) :addr_(htonl(addr)) {}

	std::string to_string() const;
	in_addr_t raw() const;

private:
	void init(const char* addr);

	in_addr_t addr_;
};


}
}