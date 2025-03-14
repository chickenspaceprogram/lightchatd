#include <sys/epoll.h>
#include <cerrno>
#include <cstdint>
#include <system_error>
#include "poller.hpp"

constexpr uint32_t EPOLL_FLAGS_ARR[] = {EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP};

namespace SockLib {

Poller::Poller() {
    errno = 0;
    pollfd = epoll_create(1234);
    if (pollfd == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to create epoll instance.");
    }
}

Poller &Poller::operator=(Poller &&poller) {
    if (pollfd != -1) {
        close(pollfd);
    }
    pollfd = poller.pollfd;
    poller.pollfd = -1;
    return *this;
}

int Poller::add(int fd, std::uint32_t flags) {
    errno = 0;
    struct epoll_event ep_event = {
        .events = flags,
        .data = {.fd = fd},
    };
    return ::epoll_ctl(pollfd, EPOLL_CTL_ADD, fd, &ep_event);
}

int Poller::remove(int fd) {
    errno = 0;
    return ::epoll_ctl(pollfd, EPOLL_CTL_DEL, fd, NULL);
}

int Poller::edit(int fd, std::uint32_t flags) {
    errno = 0;
    struct epoll_event ep_event = {
        .events = flags,
        .data = {.fd = fd},
    };
    return ::epoll_ctl(pollfd, EPOLL_CTL_MOD, fd, &ep_event);
}

PollResult Poller::poll(int timeout) {
    struct epoll_event ep_event;
    int result = -1;
    do {
        errno = 0;
        result = epoll_wait(pollfd, &ep_event, 1, timeout);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        return PollResult{.fd = -1,};
    }

    PollResult poll_result;
    poll_result.fd = ep_event.data.fd;
    for (uint32_t flag : EPOLL_FLAGS_ARR) {
        if (flag & ep_event.events) {
            poll_result.flags.push_back(flag);
        }
    }
    return poll_result;
}

}
