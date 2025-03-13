
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

#include "buffer.hpp"


static size_t min_buf_size(size_t data_len) {
    if (data_len == 0) {
        return 1;
    }

    size_t result = 1;
    while (data_len != 0) {
        data_len >>= 1;
        result <<= 1;
    }
    return result;
}

namespace SockLib {

void *Buffer::reserve(size_t bytes_to_reserve) {
    if (alloc_start == NULL) {
        // this is a new buffer
        capacity = min_buf_size(bytes_to_reserve);
        alloc_start = malloc(capacity);
        return alloc_start;
    }

    if (start_position == 0 && capacity - end_position > bytes_to_reserve) {
        // enough space in buffer already
        return (char *)alloc_start + end_position;
    }

    // this might be able to be optimized with calls to realloc() instead of malloc() / memcpy()

    size_t temp_capacity = min_buf_size(bytes_to_reserve);
    void *temp = malloc(temp_capacity);

    if (temp != NULL) {
        memcpy(temp, (char *)alloc_start + start_position, end_position - start_position);
        free(alloc_start);
        alloc_start = temp;
        capacity = temp_capacity;
        end_position -= start_position;
        start_position = 0;

        temp = (char *)alloc_start + end_position; // stupid
    }
    return temp;
}

}
