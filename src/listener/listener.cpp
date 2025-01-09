// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <system_error>
#include <sys/errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h> // no i haven't learned C++ io yet why do you ask

#include "listener.hpp"


/**
 * Major credit to this guide: https://beej.us/guide/bgnet/html/
 * Also, both the BSD and Linux manpages for getaddrinfo(3), socket(2), setsockopt(2), bind(2), listen(2), and accept(2) were pretty helpful.
 *
 * The unix socket API is pretty painful to deal with, but the above resources helped a lot.
 * Also, apologies for the excessive comments here, but I'll *definitely* forget how this all works if I don't explain everything.
 */

PortListener::PortListener(const char *port, int backlog) : backlog(backlog), port(port), fd_is_blocking(true) {
    struct addrinfo port_addrinfo_hints = {
        .ai_family = AF_UNSPEC, // .ai_family determines whether we are using IPv4 or IPv6, in this case we don't care
        .ai_socktype = SOCK_STREAM, // .ai_socktype determines whether the socket is TCP or UDP
        .ai_flags = AI_PASSIVE, // this, in combination with passing NULL as the first parameter to getaddrinfo(3), makes getaddrinfo return sockaddrs that are suitable for a server
    };
    struct addrinfo *port_addrinfo_list = NULL; // setting this to NULL for sanity reasons
    int _true = 1; // necessary for setsockopt()
    bool success_flag = false;
    errno = 0;
    int getaddrinfo_status = getaddrinfo(NULL, port, &port_addrinfo_hints, &port_addrinfo_list);
    
    if (getaddrinfo_status != 0) {
        fprintf(stderr, (const char *)"ERROR: getaddrinfo failed in ./src/listener/listener.cpp"\n);
        throw std::system_error(getaddrinfo_status, std::generic_category()); // if getaddrinfo fails, something is borked
                                                                              // exceptions are kinda bad but in this case we just want the program to end and maybe print an error message letting the server admin know they have a skill issue
    }
    
    // ugly for-loop that iterates through every element of port_addrinfo_list
    for (struct addrinfo *addrinfo_list = port_addrinfo_list; addrinfo_list->ai_next != NULL; addrinfo_list = addrinfo_list->ai_next) {

        // .ai_family should be either PF_INET or PF_INET6, depending on whether we're using IPv4 or IPv6
        // .ai_socktype should be SOCK_STREAM
        // .ai_protocol should be something specific to the thing we got from getaddrinfo (i think? not 100% sure)
        socket_descriptor = socket(addrinfo_list->ai_family, addrinfo_list->ai_socktype, addrinfo_list->ai_protocol);
        if (socket_descriptor == -1) {
            continue; // if socket() fails, we just try again with the next element of the linked list
        }
        
        // allows for us to reuse a port even if another process still holds onto the socket after exiting
        if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &_true, sizeof(int)) != 0) {
            fprintf(stderr, (const char *)"ERROR: setsockopt failed in ./src/listener/listener.cpp\n");
            throw std::system_error(errno, std::generic_category()); // if we cant set the sockopt a major issue has occurred
        }

        // getaddrinfo kinda just magically does the hard work for us and gives us the stuff to throw into bind()
        if (bind(socket_descriptor, addrinfo_list->ai_addr, addrinfo_list->ai_addrlen) != 0) {
            continue; // try again with the next element
        }
        success_flag = true; // we succeeded in binding the socket, yay
        break;
    }

    freeaddrinfo(port_addrinfo_list);
    if (success_flag == false) {
        fprintf(stderr, "ERROR: could not bind to port. Failed in ./src/listener/listener.cpp\n");
        throw std::system_error(0, std::generic_category()); // we looped through all the connections in the linked list and still didn't succeed in connecting
    }
    if (listen(socket_descriptor, backlog) != 0) {
        fprintf(stderr, "ERROR: could not listen on port. Failed in ./src/listener/listener.cpp\n");
        throw std::system_error(errno, std::generic_category()); // if listen() fails something major went wrong
    }
}

PortListener::~PortListener() {
    close(socket_descriptor);
}

int PortListener::getNewConnection() {
    errno = 0;
    int accept_result = accept(socket_descriptor, NULL, NULL);

    if (accept_result == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return -1;
        }
        fprintf(stderr, "ERROR: could not accept connection. Failed in ./src/listener/listener.cpp\n");
        throw std::system_error(errno, std::generic_category());
    }

    return accept_result;
}

void PortListener::setBlockingState(bool blocking_state) {
    fd_is_blocking = blocking_state;
    errno = 0;
    int fd_status_flags = fcntl(socket_descriptor, F_GETFL, 1234);

    if (fd_status_flags == -1) {
        fprintf(stderr, "ERROR: could not set fd status flags. Failed in ./src/listener/listener.cpp\n");
        throw std::system_error(errno, std::generic_category());
    }

    if (fd_is_blocking) {
        fd_status_flags |= O_NONBLOCK;
    }
    else {
        fd_status_flags &= ~O_NONBLOCK;
    }

    fd_status_flags = fcntl(socket_descriptor, F_SETFL, fd_status_flags);
          
    if (fd_status_flags == -1) {
        fprintf(stderr, "ERROR: could not set fd status flags. Failed in ./src/listener/listener.cpp\n");
        throw std::system_error(errno, std::generic_category());
    }
}
 
