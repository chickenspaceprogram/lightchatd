// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>
#include <mutex>
#include <thread>
#include "msgqueue.hpp"
#include "databuf.hpp"
// A mostly-thread-safe automatic message sender. 

class MsgSender {
    public:
        MsgSender();
        ~MsgSender();
        // the following return -1 on error and set errno
        void send(int fd, DataBuf &msg);
        int add(int fd);
        int remove(int fd);
    private:
        void sender(void);
        std::unordered_map<int, MsgQueue> queue_map;
        int epoll_fd;
        std::mutex map_mutex;
        std::thread worker_thread;
        int child_pipe_fd; // a pipe is used to notify the child process when it needs to end
        int parent_pipe_fd;
};
