#include <cstdlib>
#include <ctime>
#include <cerrno>
#include "sockfuncs.hpp"

extern "C" {

size_t num_to_read = 0;
size_t total_num_reads = 0;
bool do_random_size_reads = false;
bool cause_read_error = false;


size_t num_to_write = 0;
size_t total_num_writes = 0;
bool do_random_size_writes = false;
bool cause_write_error = false;

size_t times_to_throw_eintr = 0;

void seed_rand(void) {
    srand(time(NULL));
}

// validates that a valid buffer is passed to read/write
// you gotta use valgrind to detect invalid reads/writes tho
static void validate_space(void *space, size_t size) {
    volatile char temp_space = '\0';
    char *data = (char *)space;
    for (size_t i = 0; i < size; ++i) {
        temp_space = data[i];
        data[i] = temp_space;
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    ++total_num_reads;
    validate_space(buf, count); // validate space first, the entire buf should be valid regardless if we use it
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (cause_read_error) {
        errno = EBADF;
        return -1;
    }
    if (num_to_read == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    size_t max_readable = (count > num_to_read) ? num_to_read : count;
    size_t num_read = 0;
    if (do_random_size_reads) {
        num_read = rand() % max_readable + 1; // never return a 0-byte-read
    }
    else {
        num_read = max_readable;
    }
    num_to_read -= num_read;
    return num_read;
}

ssize_t write(int fd, void *buf, size_t count) {
    ++total_num_writes;
    validate_space(buf, count); // validate space first, the entire buf should be valid regardless if we use it
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (cause_write_error) {
        errno = EBADF;
        return -1;
    }
    if (num_to_write == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    size_t max_writeable = (count > num_to_write) ? num_to_write : count;
    size_t num_written = 0;
    if (do_random_size_writes) {
        num_written = rand() % max_writeable + 1;
    }
    else {
        num_written = max_writeable;
    }
    num_to_write -= num_written;
    return num_written;
}


ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    ++total_num_reads;
    size_t total = 0;
    for (int i = 0; i < iovcnt; ++i) {
        validate_space(iov[i].iov_base, iov[i].iov_len);
        total += iov[i].iov_len;
    }
    if (total == 0) {
        return 0;
    }
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (cause_read_error) {
        errno = EBADF;
        return -1;
    }
    if (num_to_read == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    size_t max_readable = (total > num_to_read) ? num_to_read : total;
    size_t num_read = 0;
    if (do_random_size_reads) {
        num_read = rand() % max_readable + 1; // never return a 0-byte-read
    }
    else {
        num_read = max_readable;
    }
    num_to_read -= num_read;
    return num_read;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    ++total_num_writes;
    size_t total = 0;
    for (int i = 0; i < iovcnt; ++i) {
        validate_space(iov[i].iov_base, iov[i].iov_len);
        total += iov[i].iov_len;
    }
    if (total == 0) {
        return 0;
    }
    if (times_to_throw_eintr > 0) {
        errno = EINTR;
        --times_to_throw_eintr;
        return -1;
    }
    if (cause_write_error) {
        errno = EBADF;
        return -1;
    }
    if (num_to_write == 0) {
        errno = (rand() % 2 == 0) ? EAGAIN : EWOULDBLOCK; // alternate returning EAGAIN/EWOULDBLOCK to check for portability
        return -1;
    }
    size_t max_writeable = (total > num_to_write) ? num_to_write : total;
    size_t num_written = 0;
    if (do_random_size_writes) {
        num_written = rand() % max_writeable + 1;
    }
    else {
        num_written = max_writeable;
    }
    num_to_write -= num_written;
    return num_written;
}

}
