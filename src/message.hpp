#pragma once
#include <cstddef>
#include <memory>
#include <sys/uio.h>

class Message {
    public:
    Message() = delete;
    Message(std::size_t size) : array(new char[size]), len(size), start_index(0) {}
    Message(const Message &) = delete;
    Message &operator=(const Message &) = delete;

    Message(Message &&) = default;
    Message &operator=(Message &&) = default;

    char &operator[](std::size_t index) { return array[index]; }
    const char &operator[](std::size_t index) const { return array[index]; }
    iovec get_iovec() { return iovec {.iov_base = array.get() + start_index, .iov_len = len - start_index}; }
    std::size_t size() const { return len; }
    void pop_front(std::size_t amount) { start_index += amount; }
    private:
    std::unique_ptr<char[]> array;
    std::size_t len;
    std::size_t start_index;
};
