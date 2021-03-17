#pragma once
#include <arpa/inet.h>
#include <string>

namespace psyche
{
namespace ip
{
using Port = std::uint16_t;

class Address
{
public:
    Address(const char* address);
    Address(const std::string& address);

    explicit Address(const uint32_t address);

    [[nodiscard]]
    std::string to_string() const;
    [[nodiscard]]
    in_addr_t raw() const;

private:
    void init(const char* addr);

    in_addr_t addr_;
};
}
}
