#include "psyche/psyche.h"
using namespace psyche;

int main() {
    Server s(9981);
    s.set_new_conn_callback([](Connection con)
    {
        LOG_INFO << "New connect from " << con.peer_endpoint().to_string() << ".";
    });
    s.set_read_callback([](Connection con, Buffer buffer)
    {
        const auto msg(buffer.retrieve_all());
        LOG_INFO << "Received from (" << con.peer_endpoint().to_string()
            << "):" << msg;
        con.send(msg);
        con.close();
    });
    s.set_close_callback([](Connection con)
    {
        LOG_INFO << con.peer_endpoint().to_string() << " connection closed";
    });
    s.start();
}
