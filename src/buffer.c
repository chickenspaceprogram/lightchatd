
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

#include <stdlib.h>
#include <string.h>

#include "buffer.h"


static size_t min_buf_size(size_t data_len) {
    if (data_len == 0) {
        return 0;
    }

    size_t result = 1;
    while (data_len != 0) {
        data_len >>= 1;
        result <<= 1;
    }
    return result;
}

void *buffer_reserve(struct buffer *buf, size_t bytes_to_reserve) {
    if (buf->alloc_start == NULL) {
        // this is a new buffer
        buf->capacity = min_buf_size(bytes_to_reserve);
        buf->alloc_start = malloc(buf->capacity);
        return buf->alloc_start;
    }

    if (buf->start_position == 0 && buf->capacity - buf->end_position > bytes_to_reserve) {
        // enough space in buffer already
        return (char *)buf->alloc_start + buf->end_position;
    }

    // this might be able to be optimized with calls to realloc() instead of malloc() / memcpy()

    size_t temp_capacity = min_buf_size(bytes_to_reserve);
    void *temp = malloc(temp_capacity);

    if (temp != NULL) {
        memcpy(temp, (char *)buf->alloc_start + buf->start_position, buf->end_position - buf->start_position);
        free(buf->alloc_start);
        buf->alloc_start = temp;
        buf->capacity = temp_capacity;
        buf->end_position -= buf->start_position;
        buf->start_position = 0;

        temp = (char *)buf->alloc_start + buf->end_position; // stupid
    }
    return temp;
}

void buffer_write(struct buffer *buf, size_t bytes_written) {
    buf->end_position += bytes_written;
}

void *buffer_start(struct buffer *buf) {
    return (char *)buf->alloc_start + buf->start_position;
}

size_t buffer_bytes_readable(struct buffer *buf) {
    return buf->end_position - buf->start_position;
}

void buffer_read(struct buffer *buf, size_t bytes_read) {
    buf->start_position += bytes_read;
}

void buffer_new(struct buffer *buf) {
    buf->alloc_start = NULL;
    buf->start_position = 0;
    buf->end_position = 0;
    buf->capacity = 0;
}

void buffer_free(struct buffer *buf) {
    if (buf->alloc_start != NULL) {
        free(buf->alloc_start);
        buf->alloc_start = NULL;
    }
    buf->start_position = 0;
    buf->end_position = 0;
    buf->capacity = 0;
}
