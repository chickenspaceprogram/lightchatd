#include "notifier.hpp"

namespace SockLib {

void Notifier::reap() {
    for (auto iter = users.begin(); iter != users.end(); ) {
        if (iter->expired()) {
            iter = users.erase(iter);
        }
        else {
            ++iter;
        }
    }
}


std::vector<std::shared_ptr<SocketHandler>> Notifier::broadcast(const std::vector<char> &message) { // returns the sockets with errors
    std::vector<std::shared_ptr<SocketHandler>> errors;
    for (auto iter = users.begin(); iter != users.end(); ) {
        auto ptr = iter->lock();
        if (ptr == nullptr) {
            iter = users.erase(iter);
        }
        else {
            if (ptr->flush() == -1) {
                errors.push_back(ptr);
                iter = users.erase(iter);
            }
            else if (ptr->write(&(message[0]), message.size()) == -1) {
                errors.push_back(ptr);
                iter = users.erase(iter);
            }
        }
    }
    return errors;
}

}
