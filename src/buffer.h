
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

#include <stddef.h>

struct buffer {
    // private 
    void *alloc_start; 
    size_t start_position;
    size_t end_position;
    size_t capacity;
};


/*
 * Ensures that at least `bytes_to_reserve` bytes can be written to the buffer pointed to by `buf`.
 *
 * Returns a pointer to the first uninitialized byte in the buffer.
 * You'll want to pass this pointer to read(2) or whatever.
 *
 * This pointer may be NULL if malloc(3) fails.
 * If this pointer is NULL, you **still need to free the buffer** with a call to buffer_free().
 */
void *buffer_reserve(struct buffer *buf, size_t bytes_to_reserve);

/*
 * Moves the end position of the buffer `bytes_written` bytes further along.
 * It is undefined behavior to call this function without first reserving at least `bytes_written` bytes.
 *
 * It's intended to call this function after calling read(2) or similar.
 * read(2) reads some number of bytes into the pointer given to you by `buffer_reserve`, and then you can call this function to advance the buffer that number of bytes.
 */
void buffer_write(struct buffer *buf, size_t bytes_written);

/*
 * Gets a pointer to the start of the buffer.
 * If the buffer is empty, NULL will be returned. Otherwise, it is guaranteed that NULL will not be returned.
 */
void *buffer_start(struct buffer *buf);

/*
 * Returns the number of bytes that are in the buffer available to be read.
 */
size_t buffer_bytes_readable(struct buffer *buf);

/*
 * Moves the start position of the buffer `bytes_read` bytes further along.
 * Effectively, this frees the first `bytes_read` bytes of the buffer.
 * It is undefined to access these bytes after this function is called.
 */
void buffer_read(struct buffer *buf, size_t bytes_read);

/*
 * Initializes a new buffer at the location pointed to by `buf`.
 * This function does not itself allocate any memory and thus is guaranteed to succeed.
 */
void buffer_new(struct buffer *buf);

/*
 * Frees the buffer pointed to by `buf`.
 * `buf` will be set to the same state as a buffer created by buffer_new.
 */
void buffer_free(struct buffer *buf);

