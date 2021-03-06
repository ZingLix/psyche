#pragma once
#include <vector>
#include <string>

namespace psyche
{
class BufferImpl
{
public:
    BufferImpl();
    BufferImpl(BufferImpl&& other) noexcept;

    const char* start();
    const char* end();

    std::string retrieve(std::size_t num);
    std::string retrieve_all();
    std::string retrieve_until(const std::string& str);

    void check_index();
    void index_init();

    [[nodiscard]]
    size_t free_size() const;
    [[nodiscard]]
    size_t current_size() const;

    size_t read_fd(int fd);
    size_t write_fd(int fd);

    void append(const std::string& str);
    void prepend(const std::string& str);

private:
    const std::size_t k_initial_size_ = 1024;
    const std::size_t k_prepend_size_ = 8;

    std::vector<char> buffer_;
    std::size_t begin_;
    std::size_t end_;
};

class BufferWrapper
{
public:
    BufferWrapper(BufferImpl& b): buffer_(&b) {
    }

    [[nodiscard]]
    size_t available() const;
    [[nodiscard]]
    std::string retrieve(std::size_t num) const;
    [[nodiscard]]
    std::string retrieve_all() const;
    [[nodiscard]]
    std::string retrieve_until(const std::string& str) const;

private:
    BufferImpl* buffer_;
};

using Buffer = BufferWrapper;
}
