#pragma once
#include <sys/types.h>

extern "C" {

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

void seed_rand(void);


// all of the following default to 0 or false


// initialize this with a value, run one of the above functions
// the user will be allowed to read up until they call a read with more than this value, at which point they'll do a partial read and will from then on get EWOULDBLOCK
//
// if this is 0, either EWOULDBLOCK or EAGAIN will be returned
// if this is -1, a random error (not EWOULDBLOCK/EAGAIN) will be returned
extern size_t num_to_read; 
// this counter is incremented by 1 whenever read() or readv() is called
extern size_t total_num_reads;
// makes reads have random, positive sizes
extern bool do_random_size_reads;

extern size_t num_to_write; // same as num_to_read but for writes
extern size_t total_num_writes; // same as total_num_reads but for writes
extern bool do_random_size_writes; // same as do_random_size_reads but for writes

// if this value is 0, nothing happens
// if this value is greater than 0, it is decremented on a call to read/write/readv/writev and EINTR is thrown
extern size_t times_to_throw_eintr;
}

