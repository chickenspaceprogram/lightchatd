// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <utility>
#include <sys/socket.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <system_error>
#include "msgqueue.hpp"

// credit to the manpage for `fcntl` for explaining how to do this
int make_fd_nonblocking(int fd) {
    errno = 0;
    int fd_status_flags = fcntl(fd, F_GETFL, 1234);
    if (fd_status_flags == -1) {
        return -1;
    }
    fd_status_flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, fd_status_flags) == -1) {
        return -1;
    }

    return 0;
}

MsgQueue::MsgQueue(int fd) : fd(fd) {
    if (make_fd_nonblocking(fd) != 0) {
        throw std::system_error(errno, std::generic_category());
    }
}

void MsgQueue::addMsg(DataBuf &msg) {
    std::unique_lock<std::mutex> lock{queue_mutex};
    queue.push(std::move(msg));
}

MsgSendStatus MsgQueue::trySendMsg() {
    std::unique_lock<std::mutex> lock{queue_mutex};

    if (isEmpty()) {
        return MsgSendStatus::NoMsgInQueue;
    }
    
    errno = 0;
    ssize_t bytes_sent = send(fd, (void *)(queue.front().buf + queue.front().index), queue.front().getSize() - queue.front().index, 0);
    if (bytes_sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return MsgSendStatus::SendWouldBlock;
        }
        else if (errno == EPIPE) {
            return MsgSendStatus::ReaderClosed;
        }
        else {
            throw std::system_error(errno, std::generic_category());
        }
    }

    if ((size_t)bytes_sent != queue.front().getSize() - queue.front().index) { // typecast that ignores warnings will never backfire ever don't worry :3
        queue.front().index += bytes_sent;
        return MsgSendStatus::NotFullySent;
    }

    queue.pop();
    return MsgSendStatus::Success;
}
