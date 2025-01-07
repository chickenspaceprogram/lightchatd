// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "msgsender.hpp"
#include <sys/epoll.h>
#include <unistd.h>


static const struct epoll_event EPOLL_WRITE_EVENT = {.events = EPOLLOUT | EPOLLRDHUP | EPOLLET};


MsgSender::MsgSender() {
    epoll_fd = epoll_create1(0);
}

MsgSender::~MsgSender() {
    close(epoll_fd);
}
