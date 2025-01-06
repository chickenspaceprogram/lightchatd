// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "databuf.hpp"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <new>

DataBuf::DataBuf(size_t len) : len(len) {
    buf = (char *)malloc(len);
    if (buf == NULL) {
        std::bad_alloc err;
        throw err; // exceptions are evil, but if malloc fails... i have bigger problems
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
