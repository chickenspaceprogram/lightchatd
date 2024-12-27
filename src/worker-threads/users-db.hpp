#ifndef WORKER_THREADS_USERS_DB_HPP // cursed, 
#define WORKER_THREADS_USERS_DB_HPP

#include <stdbool.h>
#include <string>
#include <vector>
#include <poll.h>

// a thread-safe container for a bunch of hashmaps/hashsets
class UsersDB {
    public:
        bool isInDB(std::string username);
        void addToDB(int sockfd);
    private:

};

#endif