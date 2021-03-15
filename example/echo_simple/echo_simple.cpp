#include "psyche/psyche.h"
using namespace psyche;

int main() {
    Server s(9981);
    s.set_read_callback([](Connection con, Buffer buffer)
    {
        con.send(buffer.retrieve_all());
    });
    s.start();
}
