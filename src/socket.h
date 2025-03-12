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

#include <stdbool.h>
#include <sys/types.h>
#include "buffer.h"

struct socket_handler {
    struct buffer send_buf;
    struct buffer recv_buf;
    int fd;
};

/*
 * Creates a new socket_handler that takes ownership of the socket file descriptor `fd`.
 */
void socket_new(struct socket_handler *sock_handler, int fd);

/*
 * Attempts to write `data` to the socket owned by `sock_handler`.
 * If the socket blocks, the data is added to `sock_handler->send_buf`.
 * If there is already data in `sock_handler->send_buf`, then the data will be appended  to that buffer without calling `send(2)`. 
 */
int socket_write(struct socket_handler *sock_handler, void *data, size_t data_len);

/*
 * Sends data from the socket until the data is fully sent or EAGAIN/EWOULDBLOCK is returned.
 * If a fatal error occurs, -1 will be returned and errno will be set appropriately.
 * Otherwise, the total number of bytes sent will be returned.
 */
ssize_t socket_flush(struct socket_handler *sock_handler);

/*
 * Receives data from the socket until EAGAIN/EWOULDBLOCK is returned.
 * If a fatal error occurs, -1 will be returned and errno will be set appropriately.
 * Otherwise, the total number of bytes received will be returned.
 */
ssize_t socket_recv(struct socket_handler *sock_handler);

// you can access the received bytes through sock_handler->recv_buf

void socket_close(struct socket_handler *sock_handler);
