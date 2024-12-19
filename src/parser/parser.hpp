/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#ifndef PARSER_PARSER_HPP
#define PARSER_PARSER_HPP

#include <string>
#include <vector>

typedef struct message {
    int major_version;
    int minor_version;
    std::string command;
    std::vector<std::string> args;
} Message;

// returns -1 on error, 0 on success
int parse_message(const std::string &message_text, Message &msg);

#endif