#include "socket.hpp"


ssize_t OutputBuffer::send(evutil_socket_t fd) {
    std::vector<struct iovec> iovecs;
    size_t num_sendable = (msgs.size() > IOV_MAX) ? IOV_MAX : msgs.size();
    for (std::size_t i = 0; i < num_sendable; ++i) {
        iovecs.push_back(msgs[i].getIoVec());
    }
    auto result = send_all(fd, iovecs);
    if (result.first == -1) {
        return -1;
    }
    for (ssize_t i = 0; i < result.first; ++i) {
        msgs.pop_front();
    }
    if (!msgs.empty()) {
        msgs.front().removeFront(result.second);
    }
    return 0;
}

std::pair<ssize_t, ssize_t> OutputBuffer::send_all(evutil_socket_t fd, std::vector<struct iovec> &iovecs) {
    ssize_t result = 0;
    size_t num_eintr = 0;
    size_t num_iovecs_written = 0;
    size_t iovec_start_index = 0;
    do {
        errno = 0;
        result = writev(fd, &(iovecs[num_iovecs_written]), iovecs.size() - num_iovecs_written);
        if (errno == EINTR && num_eintr++ < MAX_NUMBER_EINTR) {
            result = 0;
            continue;
        } 
        if (result <= 0) {
            break;
        }
        std::pair<ssize_t, ssize_t> num_to_pop = get_num_to_pop(&(iovecs[num_iovecs_written]), iovecs.size() - num_iovecs_written, result);
        num_iovecs_written += num_to_pop.first;
        iovec_start_index = num_to_pop.second;
        if (num_iovecs_written == iovecs.size()) { // everything sent
            return std::pair<ssize_t, ssize_t>(iovecs.size(), 0);
        }
        iovecs[num_iovecs_written].iov_base = (char *)iovecs[num_iovecs_written].iov_base + iovec_start_index;
        iovecs[num_iovecs_written].iov_len -= iovec_start_index;
    } while (result != -1);

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return std::pair<ssize_t, ssize_t>(num_iovecs_written, iovec_start_index);
    }
    else {
        return std::pair<ssize_t, ssize_t>(-1, -1);
    }
}


std::pair<ssize_t, ssize_t> OutputBuffer::get_num_to_pop(const struct iovec *iov, size_t num_iov, size_t num_read) { // this is not the C++ way of doing things but whatever
    ssize_t num_iovecs_written = 0;
    for (std::size_t i = 0; i < num_iov; ++i) {
        if (num_read < iov[i].iov_len) {
            return std::pair<ssize_t, ssize_t>(num_iovecs_written, num_read);
        }
        num_read -= iov[i].iov_len;
        ++num_iovecs_written;
    }
    return std::pair<ssize_t, ssize_t>(num_iov, 0); // we sent everything
}
