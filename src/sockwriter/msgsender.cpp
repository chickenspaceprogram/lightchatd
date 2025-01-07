// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "msgsender.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/errno.h>

#define EPOLL_WRITE_EVENT(fd_value)  {.events = EPOLLOUT | EPOLLRDHUP | EPOLLET, .data = {.fd = (int)(fd_value)}} // on a write event for this fd, this struct will get returned by epoll and we want to know which fd it is


MsgSender::MsgSender() {
    epoll_fd = epoll_create1(0);
}

MsgSender::~MsgSender() {
    close(epoll_fd);
}

int  MsgSender::add(int fd) {
    errno = 0;
    queue_map.emplace(fd, fd); // hopefully maybe works idk
    struct epoll_event fd_event = EPOLL_WRITE_EVENT(fd);
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &fd_event);
}
