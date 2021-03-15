#include "address.h"

using namespace psyche::ip;

Address::Address(const char* address): addr_() {
    init(address);
}

Address::Address(const std::string& address): addr_() {
    init(address.c_str());
}

std::string Address::to_string() const {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr_, ip, sizeof(ip));
    return std::string(ip);
}

in_addr_t Address::raw() const {
    return addr_;
}

inline void Address::init(const char* addr) {
    inet_pton(AF_INET, addr, &addr_);
    addr_ = htonl(addr_);
}
