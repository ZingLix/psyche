#pragma once
#include <string>
#include <netinet/in.h>
#include "address.h"

namespace psyche
{
class Endpoint
{
public:
    Endpoint(const std::string& ip, std::uint16_t port);
    Endpoint(const sockaddr_in& sockaddr);
    [[nodiscard]]
    ip::Address address() const;
    [[nodiscard]]
    std::uint16_t port() const;
    [[nodiscard]]
    std::string to_string() const;
    [[nodiscard]]
    sockaddr_in sockaddr() const;

private:
    ip::Address address_;
    std::uint16_t port_;
};
}
