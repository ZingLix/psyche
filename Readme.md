[中文](https://github.com/ZingLix/psyche/blob/master/Readme.zh.md)
 | English

# psyche

psyche is a modern C++ (C++11 and above) non-blocking network library for Linux and provides serveral components.

- Non-blocking network IO
- Log

## Example

A simple echo server.

``` cpp
#include "psyche/psyche.h"
using namespace psyche;

int main() {
    Server s(9981);
    s.setReadCallback([](Connection con, Buffer buffer)
    {
        con.send(buffer.retrieve());
    });
    s.start();
}
```

And an echo server which considers the complete lifetime of the connection.

``` cpp
#include "psyche/psyche.h"
using namespace psyche;

int main() {
    Server s(9981);
    s.setNewConnCallback([](Connection con)
    {
        LOG_INFO << "New connect from " << con.peer_endpoint().to_string() << ".";
    });
    s.setReadCallback([](Connection con,Buffer buffer)
    {
        auto msg(buffer.retrieveAll());
        LOG_INFO << "Received from " << con.peer_endpoint().to_string()
            << " :" << msg;
        con.send(msg);
    });
    s.setCloseCallback([](Connection con)
    {
        LOG_INFO << con.peer_endpoint().address().to_string() << ":"
            << con.peer_endpoint().port() << " connection closed";
    });
    s.start();
}
```

`Server` is a class which maintains connections. Users can set different callback funtions to different situations using the functions like `setReadCallback`. Then invoke `s.start()` and the server will start.

All callback funtions will provide a shared pointer `Connection` which points to the connection, which means users can maintain the lifetime of a connection as well besides what `Server` does.

Log library provides several levels `LOG_XXX` and `std::cout` like usage. 

You can find more samples in the [example folder](https://github.com/ZingLix/psyche/tree/master/example).

## Installation

```
cmake .
make
sudo make install
```

Or if you want to complie examples, use the commands below

```
cmake . -DPSYCHE_BUILD_EXAMPLES=ON
```

The output files will be in the `build` folder. Header files and lib will be installed into `/usr/local`.

## Thanks

Inspired by [muduo](https://github.com/chenshuo/muduo) and [asio](https://think-async.com/Asio/)

## License

MIT