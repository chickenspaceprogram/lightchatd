
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

#include <cstring>
#define LIGHTCHATD_TESTING
#include "../buffer.hpp"
#undef LIGHTCHATD_TESTING
#include "relassert.hpp"


void test_new_free(void) {
    SockLib::Buffer buf1;
}

void test_rw(void) {
    SockLib::Buffer buf;
    void *ptr = buf.reserve(1234);
    relassert(ptr != NULL);
    relassert(buf.alloc_start == ptr);
    relassert(buf.alloc_start == buf.start());
    relassert(buf.start_position == 0);
    relassert(buf.end_position == 0);
    relassert(buf.capacity > 1234);

    buf.write(456);
    relassert(buf.alloc_start == ptr);
    relassert(buf.alloc_start == buf.start());
    relassert(buf.start_position == 0);
    relassert(buf.end_position == 456);
    relassert(buf.capacity > 1234);
    relassert(buf.num_bytes() == 456);

    buf.read(123);
    relassert((char *)buf.alloc_start + 123 == (char *)buf.start());
    relassert(buf.start_position == 123);
    relassert(buf.end_position == 456);
    relassert(buf.capacity > 1234);
    relassert(buf.num_bytes() == (456 - 123));

    ptr = buf.reserve(10000);
    relassert(ptr != NULL);
    relassert((char *)buf.alloc_start + (456 - 123) == ptr);
    relassert(buf.alloc_start == buf.start());
    relassert(buf.start_position == 0);
    relassert(buf.end_position == (456 - 123));
    relassert(buf.capacity > 1234 + 10000);
}

int main(void) {
    test_new_free();
    test_rw();
}
