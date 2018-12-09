#include "address.h"

using namespace psyche::ip;

address::address(const char* addr): addr_() {
	init(addr);
}

address::address(const std::string addr): addr_() {
	init(addr.c_str());
}

std::string address::to_string() const {
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr_, ip, sizeof(ip));
	return std::string(ip);
}

in_addr_t address::raw() const {
	return addr_;
}

inline void address::init(const char* addr) {
	inet_pton(AF_INET, addr, &addr_);
	addr_ = htonl(addr_);
}