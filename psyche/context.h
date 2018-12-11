#pragma once
#include <unordered_map>
#include <memory>
#include "epoller.h"
#include "socket.h"
#include "util.h"
#include "buffer.hpp"

namespace psyche {

class channel;

class context
{
public:
	context();

	void run();
	void set_revent(int fd, int events);
	void update_poller(int fd, int events);
	void set_read_callback(int fd, EventCallback cb, buffer* buffer);
	void set_write_callback(int fd, EventCallback cb, buffer* buffer);
	void set_error_callback(int fd, EventCallback cb, buffer* buffer);
	channel* get_channel(int fd);
	void stop();
	
	void add_channel(int fd);
	void remove_channel(int fd);
	
private:
	bool running_;
	bool quit_;
	std::map<int, channel> channel_map_;
	std::unique_ptr<epoller> epoller_;
	std::vector<int> fd_list_;
};
}