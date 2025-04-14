#pragma once
#include <unordered_map>
#include <memory>
#include <event2/event.h>
#include "connection.hpp"

template <std::size_t MAX_SIZE>
class Notifier {
    public:
    void add(std::shared_ptr<Connection<MAX_SIZE>> socket);
    void remove(evutil_socket_t socket);
    bool send(std::shared_ptr<Message> msg) {
        for (auto sock : socks) {
            sock.second().add_to_send(msg);
            if (sock.second().flush_send() == -1) {
                socks.erase(sock.first());
                return false; // bad but hey it's fine
            } // bad but hey it's fine
        }
        return true;
    }
    private:
    std::unordered_map<evutil_socket_t, std::shared_ptr<Connection<MAX_SIZE>>> socks;
};
