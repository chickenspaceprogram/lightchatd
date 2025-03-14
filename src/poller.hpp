
// Copyright 2024, 2025 Athena Boose

// This file is part of lightchatd.

// lightchatd is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.

// lightchatd is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
// for more details.

// You should have received a copy of the GNU Affero General Public License
// along with lightchatd. If not, see <https://www.gnu.org/licenses/>. 

#pragma once

#include <cstdint>
#include <vector>
#include <unistd.h>

namespace SockLib {

struct PollResult {
    public:
    std::vector<uint32_t> flags;
    int fd;
};

class Poller {
    // This is a very thin wrapper around epoll, mostly just something that will automatically call the destructor and figure out which flags were returned.
    public:
    Poller();

    Poller(const Poller &) = delete;
    Poller &operator=(const Poller &) = delete;

    Poller(Poller &&poller) : pollfd(poller.pollfd) { poller.pollfd = -1; }
    Poller &operator=(Poller &&poller);

    inline ~Poller() { if (pollfd != -1) {close(pollfd);} } 


    int add(int fd, std::uint32_t flags);
    int remove(int fd);
    int edit(int fd, std::uint32_t flags);

    PollResult poll(int timeout = -1);
    inline PollResult tryPoll() { return poll(0); }

    private:
    int pollfd;
};

}
