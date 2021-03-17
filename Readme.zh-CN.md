中文 | [English](https://github.com/ZingLix/psyche)

# psyche

[![Build Status](https://travis-ci.com/ZingLix/psyche.svg?branch=master)](https://travis-ci.com/ZingLix/psyche) ![github actions badge](https://github.com/ZingLix/psyche/actions/workflows/cmake.yml/badge.svg)

psyche 是在 Linux 下利用 C++17 开发的异步非阻塞式网络库，提供如下组件

- [非阻塞异步网络 IO](#网络)
- [日志](#日志)
- [线程池](#线程池)

## 示例

为使用 psyche 你需要先 `#include <psyche/psyche.h>` 或者只包含所需的组件头文件。

### 网络

psyche 支持 TCP 协议的服务器端和 ~~客户端~~（Todo）。

下面是一个简易的 echo 服务器。

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

以及一个涉及到了一个连接完整生命周期的 echo 服务器。

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

`Server` 是一个负责管理连接的类，可以为其设置不同情况的默认回调函数，如 `setReadCallback`，然后开启服务器 `s.start()` 即可。`Server` 会用线程池调用并维护接收和发送的回调函数，但每一连接同时只会有一个接收回调函数被调用。

所有回调函数都会提供 `Connection` 作为参数，连接关闭后 `Server` 将不再拥有该 `Connection`。用户也同样可以持有 `Connection` ，当引用计数为 0 时资源便会释放（类似 `shared_ptr`）。当关闭的回调函数被调用后不应当再拥有该 `Connection`。

接收信息的回调函数有一参数 `Buffer` ，其中保存了接收到的数据，可以用 `retrieve(num)` 获得一定长度字节的数据或者用 `retrieveAll()` 获得所有数据。

### 日志

可以用 `#include <psyche/log.h>` 来只使用日志库。

日志库提供了类似于 `std::cout` 的使用方式。

``` cpp
LOG_INFO << "New connect from " << con.peer_endpoint().to_string() << ".";
LOG_DEBUG << "Example" << msg;
```

从低到高总共有 `LOG_TRACE`、`LOG_DEBUG`、`LOG_INFO`、`LOG_WARN`、`LOG_ERROR`、`LOG_FATAL` 六个级别。

### 线程池

可以用 `#include <psyche/thread_pool.h>` 来只使用线程池。

``` cpp
ThreadPool tp;        //Create a thread pool with default count of threads
ThreadPool tp(16);    //Create a thread pool with 16 threads
tp.execute([](){ LOG_INFO << "Thread started." ;});  //Invoke lambda function
auto result = tp.execute(Func, arg1, arg2);          //Invoke functions with arguments
result.get();   //Get the result of one task
```

默认构造的线程池中线程数量与硬件相关（`std::thread::hardware_concurrency()`）或者可以自己制定线程数量。

调用 `Execute()` 以添加任务，可以传递 lambda 表达式，之后参数会被传递至提供的函数。调用返回值可以用 `get()` 获得返回值。

当 `ThreadPool` 被析构时不会继续运行仍处于任务队列中的任务，但会等待已在运行的任务。可以调用 `stop()` 显式的等待所有的任务完成。

[example 文件夹](https://github.com/ZingLix/psyche/tree/master/example)中会有更多示例。

## 项目

利用 psyche 开发的项目：

- [psyche-http](https://github.com/ZingLix/psyche-http) ：基于 psyche 的 HTTP 服务器
- [LixTalk](https://github.com/ZingLix/LixTalk-server) ：即时通讯软件服务器端

## 安装

在 Ubuntu 16.04 下的 gcc-6 和 clang-6 及更高版本的编译器上通过编译。

```
cmake .
make
sudo make install
```

或者可以用下面这条命令同时编译所有或者特定示例（在 example 文件夹下的文件夹名）

```
cmake . -DPSYCHE_BUILD_ALL_EXAMPLES=ON
cmake . -DPSYCHE_BUILD_EXAMPLE="echo"
```

生成的文件位于 `build` 文件夹中，头文件和库会被安装至 `/usr/local/` 下。

## Thanks

本项目受 [muduo](https://github.com/chenshuo/muduo) 和 [asio](https://think-async.com/Asio/) 启发而来

## License

MIT
