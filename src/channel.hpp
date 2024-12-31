#pragma once

#include <unordered_set>
#include <string>
#include <shared_mutex>

// a thread-safe channel
class Channel {
    public:
        void msg(std::string message);
        void add(int fd);
        void remove(int fd);
    private:
        std::unordered_set<int> fd_set;
        std::shared_mutex fd_set_mutex;
};