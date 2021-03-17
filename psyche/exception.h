#pragma once
#include <stdexcept>
#include <cstring>

namespace psyche
{
class ErrnoException:public std::exception
{
public:
    explicit ErrnoException(int errno_):errno_(errno_){}

    [[nodiscard]]
    const char* what() const noexcept override
    {
        return std::strerror(errno);
    }

private:
    int errno_;
};

constexpr void check_errno(const int fd) {
    if(fd==-1) {
        throw ErrnoException(errno);
    }
}
}
