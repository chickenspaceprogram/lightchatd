#include "listener.hpp"
#include <utility>

Listener::ExitStatus Listener::startListening() {
    int return_val = event_base_loop(event_base, 0);
    switch (return_val) {
        case 1:
            return NoMoreEvents;
        case -1:
            return BackendErr;
        case 0:
            return ExitedNormally;
        default:
            return BackendErr; // should never happen
    }
}

Listener::ExitStatus Listener::stopListening() {
    if (event_base_loopexit(event_base, NULL) == 0) {
        return ExitedNormally;
    }
    return BackendErr;
}

Listener::ExitStatus Listener::add(std::shared_ptr<Event> event, evutil_socket_t fd, short when_to_notify) {
    
}

bool operator<(std::pair<evutil_socket_t, short> a, std::pair<evutil_socket_t, short> b) {
    if (a.first < b.first) {
        return true;
    }
    if (a.first == b.first && a.second < b.second) {
        return true;
    }
    return false; // a.first > b.first || a.first == b.first && a.second >= b.second
}

