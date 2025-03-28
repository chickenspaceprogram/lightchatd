// Copyright 2024, 2025 Athena Boose

// This file is part of lightchatd.

// lightchatd is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.

// lightchatd is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
// for more details.

// You should have received a copy of the GNU Affero General Public License
// along with lightchatd. If not, see <https://www.gnu.org/licenses/>. 

#pragma once

#include <cstdbool>
#include <sys/types.h>
#include <utility>
#include <unistd.h>
#include <sys/socket.h>
#include "buffer.hpp"

namespace SockLib {

class SocketHandler {
    public:
    SocketHandler() = delete;

    /*
     * Creates a new socket_handler that takes ownership of the socket file descriptor `fd`.
     */
    inline SocketHandler(int fd) : send_buf(), recv_buf(), fd(fd) {}

    SocketHandler(const SocketHandler &) = delete;
    SocketHandler &operator=(const SocketHandler &) = delete;

    SocketHandler(SocketHandler &&handler) : send_buf(std::move(handler.send_buf)), recv_buf(std::move(handler.recv_buf)), fd(handler.fd) { handler.fd = -1; }
    SocketHandler &operator=(SocketHandler &&handler);
    ~SocketHandler() { shutdown(fd, SHUT_RDWR); }

    /*
     * Attempts to write `data` to the socket owned by `sock_handler`.
     * If the socket blocks, the data is added to `sock_handler->send_buf`.
     * If there is already data in `sock_handler->send_buf`, then the data will be appended  to that buffer without calling `send(2)`. 
     */
    int write(const void *data, std::size_t data_len);
    
    /*
     * Sends data from the socket until the data is fully sent or EAGAIN/EWOULDBLOCK is returned.
     * If a fatal error occurs, -1 will be returned and errno will be set appropriately.
     * Otherwise, the total number of bytes sent will be returned.
     */
    ssize_t flush();

    /*
     * Receives data from the socket until EAGAIN/EWOULDBLOCK is returned.
     * If a fatal error occurs, -1 will be returned and errno will be set appropriately.
     * Otherwise, the total number of bytes received will be returned.
     */
    ssize_t recv();

    // gets a pointer to the incoming bytes
    inline void *getIncomingBuf() { return recv_buf.start(); }
    inline std::size_t getNumIncoming() const { return recv_buf.num_bytes(); }
    inline void emptyIncomingBuf() { recv_buf.free_buf(); }

    private:
    Buffer send_buf;
    Buffer recv_buf;
    int fd;
};

}
