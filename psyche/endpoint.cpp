#include "endpoint.h"
#include <string>
#include <arpa/inet.h>

using namespace psyche;

Endpoint::Endpoint(const std::string& ip, std::uint16_t port)
    : address_(ip), port_(port) {
}

Endpoint::Endpoint(const sockaddr_in& sockaddr)
    : address_(sockaddr.sin_addr.s_addr),
      port_(ntohs(sockaddr.sin_port)) {
}


ip::Address Endpoint::address() const {
    return address_;
}

std::uint16_t Endpoint::port() const {
    return port_;
}

sockaddr_in Endpoint::sockaddr() const {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port());
    addr.sin_addr.s_addr = htonl(address().raw());
    return addr;
}

std::string Endpoint::to_string() const {
    return address().to_string() + ":" + std::to_string(port());
}
