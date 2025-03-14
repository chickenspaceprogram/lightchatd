#pragma once

#include <string>
#include <memory>
#include "socket.hpp"

namespace SockLib {

class User {
    public:
    User() = delete;
    inline User(SocketHandler &&sock, std::string &&username) noexcept : sock(std::move(sock)), username(std::move(username)) {}
    
    SocketHandler sock; // storing these by value because `User` is basically a box that owns a socket and a user.
    std::string username;
};

}
