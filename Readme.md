[中文](https://github.com/ZingLix/psyche/blob/master/Readme.zh-CN.md)
 | English

# psyche

[![Build Status](https://travis-ci.com/ZingLix/psyche.svg?branch=master)](https://travis-ci.com/ZingLix/psyche) ![github actions badge](https://github.com/ZingLix/psyche/actions/workflows/cmake.yml/badge.svg)

psyche is a non-blocking network library for Linux developed with C++17 and provides several components.

- [Non-blocking network IO](#Network)
- [Log](#Log)
- [Thread Pool](#Thread-Pool)

## Example

To use psyche you need to `#include <psyche/psyche.h>` or the header file for the specified component.

### Network

psyche supports server and ~~client~~ (todo) based on TCP.

This is a simple echo server.

``` cpp
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
```

And an echo server which considers the complete lifetime of the connection.

``` cpp
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
```

`Server` is a class which owns and maintains connections. Users can set different default callback functions to different situations using the functions like `setReadCallback`. Then invoke `s.start()` and the server will start. `Server` uses a thread pool to invoke read and send callback functions, but only one read callback functions will be invoked at one time for each `Connection`.

All callback functions will provide `Connection` as an argument. When the connection is closed, `Server` will not own `Connection` anymore. Users can own the `Connection` as well. When the numbers of `Connections` instances is zero, the resources will be released (like `shared_ptr`). When the close callback function is invoked, users should not own the `Connection` instance anymore. 

Read callback provides `Buffer` as an argument which stores the data received. User can invoke `retrieve(num)` to retrieve specified length data or `retrieveAll()` to retrieve all data.

### Log

`#include <psyche/log.h>` to use log library only.

Log library provides  `std::cout` like usage. 

``` cpp
LOG_INFO << "New connect from " << con.peer_endpoint().to_string() << ".";
LOG_DEBUG << "Example" << msg;
```

It provides `LOG_TRACE`, `LOG_DEBUG`, `LOG_INFO`, `LOG_WARN`, `LOG_ERROR` and `LOG_FATAL` six levels from low to high.  

### Thread Pool

`#include <psyche/thread_pool.h>` to use thread pool only.

``` cpp
ThreadPool tp;        //Create a thread pool with default count of threads
ThreadPool tp(16);    //Create a thread pool with 16 threads
tp.execute([](){ LOG_INFO << "Thread started." ;});  //Invoke lambda function
auto result = tp.execute(Func, arg1, arg2);          //Invoke functions with arguments
result.get();   //Get the result of one task
```

Thread pool constructed with default constructor will create threads which quantity related to the hardware (`std::thread::hardware_concurrency()`). Otherwise, you can specify the number of threads as well.

Invoke `Execute()` to add tasks. You can pass lambda expression and arguments as well. You can get the return value and call `get()` to get the return value of your task.

When `ThreadPool` is destroyed, the tasks in the task queue will not be executed. But the destructor will wait for all the running tasks to finish. You can call `stop()` to wait for all the tasks to finish explicitly.

You can find more samples in the [example folder](https://github.com/ZingLix/psyche/tree/master/example).

## Project

Projects based on psyche:

- [psyche-http](https://github.com/ZingLix/psyche-http) ：HTTP Server based on psyche
- [LixTalk](https://github.com/ZingLix/LixTalk-server) ：Server-side of an IM software

## Installation

Compile passed with gcc-6 and clang-6 and higher edition in Ubuntu 16.04.

```
cmake .
make
sudo make install
```

Or if you want to compile all or specified examples, use the commands below

```
cmake . -DPSYCHE_BUILD_ALL_EXAMPLES=ON
cmake . -DPSYCHE_BUILD_EXAMPLE="echo(example name)"
```

The output files will be in the `build` folder. Header files and lib will be installed into `/usr/local`.

## Thanks

Inspired by [muduo](https://github.com/chenshuo/muduo) and [asio](https://think-async.com/Asio/)

## License

MIT
