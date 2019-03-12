#pragma once
#include <set>
#include "connection.h"
#include "acceptor.h"
#include "ThreadPool.h"

namespace psyche
{
class Server
{
public:
	using NewConnCallback = std::function<void(Connection)>;
	using ReadCallback = std::function<void(Connection,Buffer)>;
	using WriteCallback = std::function<void(Connection)>;
	using CloseCallback = std::function<void(Connection)>;
	using ErrorCallback = std::function<void(Connection,error_code)>;

	Server(std::uint16_t port, const std::string& ip = "0.0.0.0");

	void setNewConnCallback(NewConnCallback cb);
	void setReadCallback(ReadCallback cb);
	void setWriteCallback(WriteCallback cb);
	void setCloseCallback(CloseCallback cb);
	void setErrorCallback(ErrorCallback cb);

	void start();
    void stop();
	void erase(connection_ptr con);

private:
    /*class ConnnectionCleanerThread
    {
    public:
        ConnnectionCleanerThread() 
            :stop_(false), thread_(std::bind(&ConnnectionCleanerThread::clean,this)) 
        {
        }

        void add(connection_ptr&& ptr) {
            std::lock_guard<std::mutex> guard(mutex_);
            rubbish_frontend_.emplace_back(std::move(ptr));
            if (rubbish_frontend_.size() > 16) cv_.notify_all();
        }

        void clean() {
            using namespace std::chrono_literals;
            while (!stop_) {
                {
                    std::unique_lock<std::mutex> lk(mutex_);
                    if (rubbish_frontend_.size() == 0) {
                        cv_.wait_for(lk, 3s);
                    }
                    rubbish_backend_.swap(rubbish_frontend_);
                }
                rubbish_backend_.clear();
            }
        }

        ~ConnnectionCleanerThread() {
            stop_ = true;
            thread_.join();
        }

    private:
        std::atomic_bool stop_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::vector<connection_ptr> rubbish_frontend_;
        std::vector<connection_ptr> rubbish_backend_;
        std::thread thread_;
    };*/

	void handleRead(Connection con, Buffer buffer) const;
	void handleWrite(Connection con) const;
	void handleClose(Connection con);

	context context_;
	acceptor acceptor_;
	std::set<connection_ptr> connections_;
  //  ConnnectionCleanerThread cleaner_thread_;

	NewConnCallback new_conn_callback_;
	ReadCallback read_callback_;
	WriteCallback write_callback_;
	CloseCallback close_callback_;
	ErrorCallback error_callback_;
};
}
