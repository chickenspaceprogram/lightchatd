
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

#include <cstddef>
#include <cstdlib>

namespace SockLib {

class Buffer {
    public:
    inline Buffer() : alloc_start(nullptr), start_position(0), end_position(0), capacity(0) {};
    
    Buffer(const Buffer &buf) = delete;
    Buffer &operator=(const Buffer &buf) = delete;

    Buffer(Buffer &&buf) : alloc_start(buf.alloc_start), start_position(buf.start_position), end_position(buf.end_position), capacity(buf.capacity) {
        buf.alloc_start = nullptr;
        buf.start_position = 0;
        buf.end_position = 0;
        buf.capacity = 0;
    }
    Buffer &operator=(Buffer &&buf) {
        free_buf();
        alloc_start = buf.alloc_start;
        start_position = buf.start_position;
        end_position = buf.end_position;
        capacity = buf.capacity;
        buf.alloc_start = nullptr;
        buf.start_position = 0;
        buf.end_position = 0;
        buf.capacity = 0;
        return *this;
    }

    ~Buffer() noexcept { free_buf(); }
    /*
     * Ensures that at least `bytes_to_reserve` bytes can be written to the buffer pointed to by `buf`.
     *
     * Returns a pointer to the first uninitialized byte in the buffer.
     * You'll want to pass this pointer to read(2) or whatever.
     *
     * This pointer may be NULL if malloc(3) fails.
     * If this pointer is NULL, you **still need to free the buffer** with a call to buffer_free().
     */
    void *reserve(std::size_t bytes_to_reserve);

    /*
     * Moves the end position of the buffer `bytes_written` bytes further along.
     * It is undefined behavior to call this function without first reserving at least `bytes_written` bytes.
     *
     * It's intended to call this function after calling read(2) or similar.
     * read(2) reads some number of bytes into the pointer given to you by `buffer_reserve`, and then you can call this function to advance the buffer that number of bytes.
     */
    inline void write(std::size_t bytes_written) noexcept { end_position += bytes_written; }

    /*
     * Gets a pointer to the start of the buffer.
     * If the buffer is empty, NULL will be returned. Otherwise, it is guaranteed that NULL will not be returned.
     */
    inline void *start() { return (char *) alloc_start + start_position; }
    inline const void *start() const { return (char *) alloc_start + start_position; }

    /*
     * Returns the number of bytes that are in the buffer available to be read.
     */
    inline std::size_t num_bytes() const { return end_position - start_position; }


    /*
     * Moves the start position of the buffer `bytes_read` bytes further along.
     * Effectively, this frees the first `bytes_read` bytes of the buffer.
     * It is undefined to access these bytes after this function is called.
     */
    inline void read(std::size_t bytes_read) {
        start_position += bytes_read;
        if (start_position == end_position) {
            free_buf();
        }
    };

    /*
     * Frees the buffer pointed to by `buf`.
     * `buf` will be set to the same state as a buffer created by buffer_new.
     */
    inline void free_buf() {
        if (alloc_start != nullptr) {
            std::free(alloc_start);
            alloc_start = nullptr;
        }
        start_position = 0;
        end_position = 0;
        capacity = 0;
    }
#ifndef LIGHTCHATD_TESTING // this sucks but i want everything to be public for testing purposes only
    private:
#endif
    void *alloc_start;
    std::size_t start_position;
    std::size_t end_position;
    std::size_t capacity;
};

}

