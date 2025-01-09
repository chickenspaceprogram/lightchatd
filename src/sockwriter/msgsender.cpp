// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/errno.h>
#include <thread>
#include <system_error>
#include "msgsender.hpp"
#include "msgqueue.hpp"


#define EPOLL_WRITE_EVENT(fd_value)  {.events = EPOLLOUT | EPOLLRDHUP | EPOLLET, .data = {.fd = (int)(fd_value)}} // on a write event for this fd, this struct will get returned by epoll and we want to know which fd it is


MsgSender::MsgSender() {
    signal(SIGPIPE, SIG_IGN); // just ignore sigpipe, this forces write/send to return an error instead of terminating the thread and being annoying
    errno = 0;
    if ((epoll_fd = epoll_create1(0)) == -1) {
        fprintf(stderr, "ERROR: could not create epoll instance. Failed in ./src/sockwriter/msgsender.cpp\n");
        throw std::system_error(errno, std::generic_category());
    } 
    int temp_arr[2] = {0};
    
    // creating pipe that allows us to send a message to the thread and unblock the epoll when desired
    if (pipe(temp_arr) == -1) {
        fprintf(stderr, "ERROR: could not create pipe. Failed in ./src/sockwriter/msgsender.cpp\n");
        throw std::system_error(errno, std::generic_category());
    }
    parent_pipe_fd = temp_arr[1];
    child_pipe_fd = temp_arr[0];
    
    // adding pipe to epoll
    struct epoll_event pipe_read_event = {.events = EPOLLIN, .data = {.fd = child_pipe_fd}};
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, child_pipe_fd, &pipe_read_event) == -1) {
        fprintf(stderr, "ERROR: could not add fd to epoll. Failed in ./src/sockwriter/msgsender.cpp\n");
        throw std::system_error(errno, std::generic_category());
    }
    // creating worker thread
    worker_thread = std::thread{&MsgSender::sender, this};
}

MsgSender::~MsgSender() {
    char buf[1] = {'a'};
    write(parent_pipe_fd, buf, 1); // sends some data to the child and unblocks the epoll(), allowing the child to notice and return
    worker_thread.join(); // waiting for worker thread to wrap up
    signal(SIGPIPE, SIG_DFL);
    close(parent_pipe_fd);
    close(child_pipe_fd);
    close(epoll_fd); // now we can close the epoll since the worker isn't actively waiting on it
}

int MsgSender::add(int fd) {
    errno = 0;
    queue_map.emplace(fd, fd); // hopefully maybe works idk
    struct epoll_event fd_event = EPOLL_WRITE_EVENT(fd);
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &fd_event);
}

int MsgSender::remove(int fd) {
    errno = 0;
    queue_map.erase(fd);
    return epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

int MsgSender::send(int fd, DataBuf &msg) {
    MsgQueue &fd_queue = queue_map[fd];
    fd_queue.addMsg(msg);
    MsgSendStatus status;
    do {
        status = fd_queue.trySendMsg();
    } while (status == MsgSendStatus::NotFullySent);

    if (status == MsgSendStatus::ReaderClosed) {
        int temp_errno = errno;
        if (remove(fd) == -1) {
            fprintf(stderr, "ERROR: the reader closed, but the fd could not be removed from MsgSender for some reason. Failed in ./src/sockwriter/msgsender.cpp\n");
            throw std::system_error(errno, std::generic_category()); // this does ignore the EPIPE but EPIPE is probably the less important error here
        }
        errno = temp_errno;
        return -1;
    }

    return 0;
}

