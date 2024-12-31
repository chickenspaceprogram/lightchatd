#include "channel.hpp"
#include <mutex>
#include <sys/socket.h>

void Channel::msg(std::string message) {
    std::shared_lock<std::shared_mutex> fd_set_mutex_sh_lock{fd_set_mutex};
    
}