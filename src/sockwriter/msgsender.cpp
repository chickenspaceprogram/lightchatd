// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <sys/epoll.h>
#include <unistd.h>
#include <sys/errno.h>
#include <thread>
#include <system_error>
#include "msgsender.hpp"


#define EPOLL_WRITE_EVENT(fd_value)  {.events = EPOLLOUT | EPOLLRDHUP | EPOLLET, .data = {.fd = (int)(fd_value)}} // on a write event for this fd, this struct will get returned by epoll and we want to know which fd it is


MsgSender::MsgSender() {
    errno = 0;
    if ((epoll_fd = epoll_create1(0)) == -1) {
        throw std::system_error(errno, std::generic_category());
    } 
    int temp_arr[2] = {0};
    
    // creating pipe that allows us to send a message to the thread and unblock the epoll when desired
    if (pipe(temp_arr) == -1) {
        throw std::system_error(errno, std::generic_category());
    }
    parent_pipe_fd = temp_arr[1];
    child_pipe_fd = temp_arr[0];
    
    // adding pipe to epoll
    struct epoll_event pipe_write_event = EPOLL_WRITE_EVENT(child_pipe_fd);
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, child_pipe_fd, &pipe_write_event)) {
        throw std::system_error(errno, std::generic_category());
    }
    // creating worker thread
    worker_thread = std::thread{&MsgSender::sender, this};
}

MsgSender::~MsgSender() {
    char buf[1] = {'a'};
    write(parent_pipe_fd, buf, 1); // sends something to the child and unblocks the epoll(), allowing the child to notice and return
    worker_thread.join(); // waiting for worker thread to wrap up
    close(epoll_fd); // now we can close the epoll since the worker isn't actively waiting on it
}

int MsgSender::add(int fd) {
    errno = 0;
    queue_map.emplace(fd, fd); // hopefully maybe works idk
    struct epoll_event fd_event = EPOLL_WRITE_EVENT(fd);
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &fd_event);
}
