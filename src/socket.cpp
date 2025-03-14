#include <sys/socket.h>
#include <errno.h>

#include "socket.hpp"

static const int SEND_FLAGS = MSG_DONTWAIT | MSG_NOSIGNAL;
static const int RECV_FLAGS = MSG_DONTWAIT;
static const size_t RECV_SIZE = 512;

namespace SockLib {

SocketHandler &SocketHandler::operator=(SocketHandler &&handler) {
    send_buf = std::move(handler.send_buf);
    recv_buf = std::move(handler.recv_buf);
    fd = handler.fd;
    handler.fd = -1;
    return *this;
}

ssize_t SocketHandler::flush() {
    ssize_t num_readable = send_buf.num_bytes();
    ssize_t return_val = 0;
    ssize_t total_num_written = 0;
    
    while (num_readable > 0) {
        errno = 0;
        return_val = send(fd, send_buf.start(), num_readable, SEND_FLAGS);
        if (return_val == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return total_num_written;
            }
            if (errno != EINTR) { // we can just retry the `send` if we get EINTR
                return -1;
            }
        }
        send_buf.read(return_val);
        total_num_written += return_val;
        num_readable -= return_val;
    }
    return total_num_written;
}

ssize_t SocketHandler::recv() {
    ssize_t num_received = 0;
    errno = 0;
    while (1) {
        void *data_ptr = recv_buf.reserve(RECV_SIZE);
        ssize_t temp = ::recv(fd, data_ptr, RECV_SIZE, RECV_FLAGS);
        if (temp == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return num_received;
            }
            else {
                return -1;
            }
        }
        recv_buf.write(temp);
        num_received += temp;
    }
    
}

}

