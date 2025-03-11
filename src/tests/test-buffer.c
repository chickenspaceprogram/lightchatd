
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

#include <string.h>
#include "../buffer.h"
#include "relassert.h"


void test_new_free(void) {
    struct buffer buf1;
    struct buffer buf2;
    buffer_new(&buf1);
    buffer_free(&buf1);
    buffer_new(&buf2);
    relassert(memcmp(&buf1, &buf2, sizeof(struct buffer)) == 0);
}

void test_rw(void) {
    struct buffer buf;
    buffer_new(&buf);
    void *ptr = buffer_reserve(&buf, 1234);
    relassert(ptr != NULL);
    relassert(buf.alloc_start == ptr);
    relassert(buf.alloc_start == buffer_start(&buf));
    relassert(buf.start_position == 0);
    relassert(buf.end_position == 0);
    relassert(buf.capacity > 1234);

    buffer_write(&buf, 456);
    relassert(buf.alloc_start == ptr);
    relassert(buf.alloc_start == buffer_start(&buf));
    relassert(buf.start_position == 0);
    relassert(buf.end_position == 456);
    relassert(buf.capacity > 1234);
    relassert(buffer_bytes_readable(&buf) == 456);

    buffer_read(&buf, 123);
    relassert((char *)buf.alloc_start + 123 == (char *)buffer_start(&buf));
    relassert(buf.start_position == 123);
    relassert(buf.end_position == 456);
    relassert(buf.capacity > 1234);
    relassert(buffer_bytes_readable(&buf) == (456 - 123));

    ptr = buffer_reserve(&buf, 10000);
    relassert(ptr != NULL);
    relassert((char *)buf.alloc_start + (456 - 123) == ptr);
    relassert(buf.alloc_start == buffer_start(&buf));
    relassert(buf.start_position == 0);
    relassert(buf.end_position == (456 - 123));
    relassert(buf.capacity > 1234 + 10000);

    buffer_free(&buf);
}

int main(void) {
    test_new_free();
    test_rw();
}
