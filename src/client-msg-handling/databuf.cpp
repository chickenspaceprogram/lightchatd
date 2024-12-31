// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "safesock.hpp"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/errno.h>

DataBuf::DataBuf(size_t len) : len(len) {
    buf = (char *)malloc(len);
    if (buf == NULL) {
        std::bad_alloc err;
        throw err;
    }
}

void DataBuf::setSize(size_t len) {
    char *next_buf = buf;
    next_buf = (char *)realloc(next_buf, len);
    if (next_buf == NULL) {
        free(buf);
        std::bad_alloc err;
        throw err;
    }

    buf = next_buf;
    this->len = len;
}

DataBuf::~DataBuf() {
    free(buf);
}

int SafeSock::send(std::string msg) {
    errno = 0;
    std::unique_lock<std::mutex> fd_lock{fd_mutex};
    int fd_flags = fcntl(fd, F_GETFL, 0);
    if (errno != 0) {
        return -1;
    }
    fd_flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, fd_flags); // fd is now nonblocking
    
}