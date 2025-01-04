// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <stddef.h>
#include <queue>
#include <mutex>
#include "databuf.hpp"

enum class MsgSendStatus {
    Success = 0,
    SendWouldBlock,
    Error,
    NoMsgInQueue,
};

// A thread-safe queue of messages waiting to be sent to a file descriptor
class MsgQueue {
    public:
        MsgQueue() = delete;
        MsgQueue(int fd);
        // whatever constructor is here needs to ensure the fd is nonblocking!

        // leaves errno as however it was set by write() for additional information
        MsgSendStatus trySendMsg(void);
        void addMsg(DataBuf &msg);
        inline bool isEmpty() {if (queue_size == 0) return true; else return false;}
        inline size_t getSize() {return queue_size;}
    private:
        size_t queue_size;
        std::queue<DataBuf> queue;
        std::mutex queue_mutex;
        int fd;
};
