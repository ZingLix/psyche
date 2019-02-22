#include "util.h"
#include "Server.h"

void psyche::setServer(Server* s) {
    curServer = s;
}

void psyche::stopServer(int) {
    curServer->stop();
}
