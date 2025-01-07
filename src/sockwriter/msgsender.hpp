// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "databuf.hpp"
#include <unordered_map>
#include "msgqueue.hpp"


class MsgSender {
    public:
        MsgSender();
        ~MsgSender();
        void send(int fd, DataBuf &msg);
        int add(int fd);
        int remove(int fd);
    private:
        std::unordered_map<int, MsgQueue> queue_map;
        int epoll_fd;
        

};
