#pragma once
#include <memory>
#include <deque>
#include <event2/event.h>

class Notifier {
    public:
    void add(evutil_socket_t socket);
    void remove(evutil_socket_t socket);
    void send(std::shared_ptr<struct iovec>);
    private:
    std::deque<std::shared_ptr<struct iovec>> messages;
};
