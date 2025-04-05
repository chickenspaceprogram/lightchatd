#pragma once
#include <cstddef>
#include <memory>
#include <deque>
#include <event2/event.h>
#include <cerrno>
#include <vector>
#include <sys/uio.h>
#include "event.hpp"

class MessageData {
    public:
    MessageData() = delete;
    MessageData(std::size_t size) : msg_ptr(new char[size]), msg_size(size) {}

    MessageData(const MessageData &) = delete;
    MessageData &operator=(const MessageData &) = delete;

    MessageData(MessageData &&data) : msg_ptr(std::move(data.msg_ptr)), msg_size(data.msg_size) {}
    MessageData &operator=(MessageData &&data) {
        msg_ptr = std::move(data.msg_ptr);
        msg_size = data.msg_size;
        return *this;
    }

    std::size_t size() const { return msg_size; }
    char &operator[](std::size_t index) { return msg_ptr[index]; }
    const char &operator[](std::size_t index) const { return msg_ptr[index]; }


    private:
    std::unique_ptr<char[]> msg_ptr;
    std::size_t msg_size;

};

class MessageSender {
    public:
    MessageSender() = delete;
    MessageSender(std::shared_ptr<MessageData> data) : data(data), start(0), end(data->size()) {}
    MessageSender(std::shared_ptr<MessageData> data, std::size_t end) : data(data), start(0), end(end) {}

    MessageSender(const MessageSender &) = delete;
    MessageSender &operator=(const MessageSender &) = delete;

    MessageSender(MessageSender &&sender) : data(sender.data), start(sender.start), end(sender.end) {}
    MessageSender &operator=(MessageSender &&sender) {
        data = sender.data;
        start = sender.start;
        end = sender.end;
        return *this;
    }

    struct iovec getIoVec() {
        return iovec {
            .iov_base = &((*data)[start]), 
            .iov_len = end - start
        };
    }

    void removeFront(std::size_t num_to_remove) { start += num_to_remove; }
    bool empty() const { return start == end; }

    private:
    std::shared_ptr<MessageData> data;
    std::size_t start;
    std::size_t end;
};

class OutputBuffer {
    public:
    OutputBuffer();
    
    OutputBuffer(const OutputBuffer &) = delete;
    OutputBuffer &operator=(const OutputBuffer &) = delete;

    OutputBuffer(OutputBuffer &&buf) : msgs(std::move(buf.msgs)) {}
    OutputBuffer &operator=(OutputBuffer &&buf) {
        msgs = std::move(buf.msgs);
        return *this;
    }

    void add(std::shared_ptr<MessageData> data) {
        msgs.push_back(MessageSender(data));
    }

    ssize_t send(evutil_socket_t fd);

    private:
    // returns <number of MessageSenders to pop, number of bytes to pop off of next MessageSender>
    static std::pair<ssize_t, ssize_t> send_all(evutil_socket_t fd, std::vector<struct iovec> &iovecs);
    static std::pair<ssize_t, ssize_t> get_num_to_pop(const struct iovec *iov, size_t num_iov, size_t num_read);

    std::deque<MessageSender> msgs;
    static constexpr std::size_t MAX_NUMBER_EINTR = 256;
};

template <std::size_t chunk_size>
class InputBuffer {
    char &operator[](std::size_t index) {
        std::size_t actual_index = index + first_elem_start_index;
        return buf[actual_index / chunk_size][actual_index % chunk_size];
    }
    const char &operator[](std::size_t index) const {
        std::size_t actual_index = index + first_elem_start_index;
        return buf[actual_index / chunk_size][actual_index % chunk_size];
    }
    void removeFront(std::size_t num_to_remove) {
        first_elem_start_index += num_to_remove;
        for (std::size_t i = 0; i < first_elem_start_index / chunk_size; ++i) {
            buf.pop_front();
        }
        first_elem_start_index %= chunk_size;
    }
    int read(evutil_socket_t fd) {
        ssize_t readval = 0;
        size_t eintr_counter = 0;
        // read until block or until the socket shits itself
        do {
            readval = read_internal(fd);
            if (errno == EINTR && eintr_counter++ < MAX_NUMBER_EINTR) {
                readval = 0;
            }
        } while (readval != -1);

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        else {
            return -1;
        }
    }
    size_t size() const { return (buf.size() - 1) * chunk_size + last_elem_end_index - first_elem_start_index; }
    private:
    int read_internal(evutil_socket_t fd) {
        errno = 0;
        if (last_elem_end_index == chunk_size) {
            buf.push_back(MessageData(chunk_size));
            last_elem_end_index = 0;
        }
        ssize_t readval = ::read(fd, &(buf.back()[last_elem_end_index]), chunk_size - last_elem_end_index);
        if (readval == -1) {
            return -1;
        }
        else {
            last_elem_end_index = readval;
            return readval;
        }
        // can probably optimize to reduce number of read syscalls but fuckit
    }
    std::deque<MessageData> buf;
    std::size_t first_elem_start_index;
    std::size_t last_elem_end_index;
    static constexpr std::size_t MAX_NUMBER_EINTR = 256;
};

class User {
    public:
    void send(std::shared_ptr<struct iovec>);
    void operator()(evutil_socket_t fd, short what) {
        if (what & EV_READ) {
             abort(); // not implemented, need to read, parse, and take necessary actions
        }
        if (what & EV_WRITE) {
            abort(); // not implemented fully
            if (send_buffer.send(fd) == -1) {
                // remove from poll
            }
        }
    }
    private:
    OutputBuffer send_buffer;
    // need to think of a sane way to handle recving
    InputBuffer<512> recv_buffer;
    evutil_socket_t fd;
};
