#pragma once
#include <functional>

namespace psyche
{
using error_code = int;
using EventCallback = std::function<void()>;

class Server;

static Server* curServer = nullptr;

void setServer(Server* s);

void stopServer(int);
}
