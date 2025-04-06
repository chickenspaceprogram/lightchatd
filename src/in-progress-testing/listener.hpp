#pragma once
#include <event2/event.h>
#include <map>
#include <memory>
#include "event.hpp"

class Listener {
    public:
    Listener();
    ~Listener();

    enum ExitStatus {
        NoMoreEvents,
        BackendErr,
        ExitedNormally, // should never actually occur here
    };

    ExitStatus startListening();

    // makes the listener stop listening gracefully
    // analogous to event_base_loopexit(event_base, NULL)
    ExitStatus stopListening();

    // kills the listener immediately after the current callback is finished
    // analogous to event_base_loopbreak(event_base)
    ExitStatus kill(); // kills the listener immediately after the current callback is finished

    // adds the event `event` to the notifier
    // any of the notifications in `when_to_notify` are added to any already-present events for the file descriptor in the Listener
    // if the event is already being listened to, nothing happens
    ExitStatus add(std::shared_ptr<Event> event, evutil_socket_t fd, short when_to_notify);
    ExitStatus remove(evutil_socket_t fd, short notification_to_remove);
    struct event_base *getEventBase() { return event_base; }

    private:
        struct event_base *event_base;
        std::map<
            std::pair<evutil_socket_t, short>,
            std::pair<std::shared_ptr<Event>, struct event *>
        > events;
};

bool operator<(std::pair<evutil_socket_t, short> a, std::pair<evutil_socket_t, short> b);
