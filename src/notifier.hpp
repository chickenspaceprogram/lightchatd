
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
#include <memory>
#include <unordered_set>
#include <vector>

#include "socket.hpp"

namespace SockLib {

class Notifier {
    public:

    void add(std::weak_ptr<SocketHandler> user) { if (!user.expired()) { users.insert(user); } }
    void remove(const std::weak_ptr<SocketHandler> &user) { users.erase(user); }
    void reap();
    std::vector<std::shared_ptr<SocketHandler>> broadcast(const std::vector<char> &message); // returns a vector of sockets with errors

    private:
    std::unordered_set<std::weak_ptr<SocketHandler>> users;
};

}
