#include <assert.h>
#include "../connection.hpp"

#define HIDE_READ_WRITE // can still link against and override read/write but gcc will complain if the definitions are there since they conflict with those in unistd.h
#include "sockfuncs.hpp"
#undef HIDE_READ_WRITE


static void test_connection_read(void) {
    seed_rand();
    Connection<512> conn(1234);
    assert(conn.receive() == 0);
    num_to_read = 1234;
    assert(conn.receive() == 512);
    assert(conn.recv_buf_size() == 512);
    assert(conn.receive() == 512);

    conn.recv_buf_pop(512);
    assert(conn.recv_buf_size() == 0);

    num_to_read = 123;
    assert(conn.receive() == 123);
    assert(conn.receive() == 123);
    
    num_to_read = 123;
    times_to_throw_eintr = 129;
    errno = 0;
    assert(conn.receive() == -1 && errno == EINTR);
    times_to_throw_eintr = 50;
    assert(conn.receive() == 123 + 123);

    times_to_throw_eintr = 0;

    cause_read_error = true;
    num_to_read = 1234;
    conn.recv_buf_pop(111);
    assert(conn.receive() == -1);
    cause_read_error = false;
    assert(conn.receive() == 512);
    
    num_to_read = 0;
    total_num_reads = 0;
}


int main(void) {
    test_connection_read();
}
