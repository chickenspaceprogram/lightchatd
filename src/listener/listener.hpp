// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.




// about all PortListener is is a non-painful way to check for new connections
// this is not necessarily thread-safe
// fd stands for *F*ile *D*escriptor if that wasn't clear
class PortListener {
    public:
        PortListener() = delete;
        PortListener(const char *port, int backlog);
        ~PortListener();
        int getNewConnection(); // if the socket descriptor is blocking, waits for a new connection and returns the associated fd
                                // if socket descriptor is nonblocking, returns the fd of the new connection if there is one, if EAGAIN or EWOULDBLOCK is returned returns -1
        void setBlockingState(bool blocking_state);
        inline bool fdIsBlocking() {return fd_is_blocking;}

    private:
        const int backlog;
        const char * const port;
        int socket_descriptor;
        bool fd_is_blocking;
};
