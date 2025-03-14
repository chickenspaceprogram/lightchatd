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
