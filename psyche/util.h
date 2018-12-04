#pragma once
#include <functional>

namespace psyche
{
using error_code = int;
using EventCallback = std::function<void(error_code, std::size_t)>;
}
