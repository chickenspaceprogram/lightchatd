#include <sys/socket.h>
#include <errno.h>

#include "socket.h"

static const int SEND_FLAGS = MSG_DONTWAIT | MSG_NOSIGNAL;
static const int RECV_FLAGS = MSG_DONTWAIT;
static const size_t RECV_SIZE = 512;


void socket_new(struct socket_handler *sock_handler, int fd) {
    buffer_new(&(sock_handler->send_buf));
    buffer_new(&(sock_handler->recv_buf));
    sock_handler->fd = fd;
}




ssize_t socket_flush(struct socket_handler *sock_handler) {
    ssize_t num_readable = buffer_bytes_readable(&(sock_handler->send_buf));
    ssize_t return_val = 0;
    ssize_t total_num_written = 0;
    while (num_readable > 0) {
        errno = 0;
        return_val = send(sock_handler->fd, buffer_start(&(sock_handler->send_buf)), num_readable, SEND_FLAGS);
        if (return_val == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return total_num_written;
            }
            if (errno != EINTR) { // we can just retry the `send` if we get EINTR
                return -1;
            }
        }
        buffer_read(&(sock_handler->send_buf), return_val);
        total_num_written += return_val;
        num_readable -= return_val;
    }
    return total_num_written;
}

ssize_t socket_recv(struct socket_handler *sock_handler) {
    ssize_t num_received = 0;
    errno = 0;
    while (1) {
        void *data_ptr = buffer_reserve(&(sock_handler->recv_buf), RECV_SIZE);
        ssize_t temp = recv(sock_handler->fd, data_ptr, RECV_SIZE, RECV_FLAGS);
        if (temp == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return num_received;
            }
            else {
                return -1;
            }
        }
        buffer_write(&(sock_handler->recv_buf), temp);
        num_received += temp;
    }
}
