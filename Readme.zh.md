中文 | [English](https://github.com/ZingLix/psyche/blob/master/Readme.md)

# psyche

psyche 是在 Linux 下利用现代 C++ （C++11 以及更高）开发的异步非阻塞式网络库，并提供相关的组件功能

- 非阻塞异步网络 IO 
- 日志

## 示例

一个简单的 echo 服务器。

``` cpp
#include "psyche/Server.h"
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

以及一个用到了一个连接完整生命周期的 echo 服务器。

``` cpp
#include "psyche/Server.h"
#include <iostream>
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

`Server` 是一个负责管理连接的类，可以为其设置不同情况的回调函数，如 `setReadCallback`，然后开启服务器 `s.start()` 即可。

所有回调函数都会提供一个连接的智能指针 `Connection`，除了 `Server` 用户也可以管理它的生命周期。

日志库提供了 `LOG_XXX` 形式多级别的，类似于 `std::cout` 的使用方式。

## 安装

```
cmake .
make
```

## Thanks

本项目受 [muduo](https://github.com/chenshuo/muduo) 启发而来

## License

MIT