/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

// i hate this parser so much lol

#include <cstddef>
#include <ctype.h>
#include "parser.hpp"
#define MAGIC_STR "LIGHTCHAT/"
#define MAGIC_STR_LEN 10

// ensures that the characters starting at `position` form the macro described in MAGIC_STR
// returns 0 if success, -1 if the magic string could not be recognized.
// increments position to the index of the first character after the magic string
int check_magic_str(const std::string &message_text, size_t &position);


// stores version in `msg`, increments `position` to first char after version.
// returns -1 on failure, 0 on success
int get_version(const std::string &message_text, size_t &position, Message &msg);

// returns -1 if the character at `position` was not a digit, returns the value of the number otherwise
// basically a specialized version of atoi()
int parse_number(const std::string &message_text, size_t &position);

int get_command(const std::string &message_text, size_t &position, Message &msg);

std::string get_arg(const std::string &message_text, size_t &position);

std::string get_text(const std::string &message_text, size_t &position);

int parse_args(const std::string &message_text, size_t &position, Message &msg);


int check_magic_str(const std::string &message_text, size_t &position) {
    char magic[] = MAGIC_STR;
    size_t msg_len = message_text.size();
    for (int i = 0; i < MAGIC_STR_LEN; ++i) {
        if (message_text[position] != magic[i] || msg_len <= position) {
            return -1;
        }
        ++position;
    }
    return -1;
}

int get_version(const std::string &message_text, size_t &position, Message &msg) {
    msg.major_version = parse_number(message_text, position);
    if (msg.major_version == -1) {
        return -1;
    }
    if (message_text[position] != '.' || message_text.size() <= position) {
        return -1;
    }
    ++position;
    msg.minor_version = parse_number(message_text, position);
    if (msg.minor_version == -1) {
        return -1;
    }
    return 0;
}

int parse_number(const std::string &message_text, size_t &position) {
    size_t msg_len = message_text.size();
    int num = 0;
    if (!isdigit(message_text[position]) || msg_len <= position) {
        return -1;
    }

    while (isdigit(message_text[position]) ) {
        if (msg_len <= position) {
            return -1;
        }
        num += message_text[position] - '0';
        num *= 10;
        ++position;
    }
    return num;
}

int get_command(const std::string &message_text, size_t &position, Message &msg) {
    size_t len = message_text.size();
    while (
        message_text[position] != ' ' && 
        message_text[position] != '\r' && 
        message_text[position] != '\n' &&
        message_text[position] != ':'
    ) {
        if (len <= position) {
            return -1;
        }
        msg.command.push_back(message_text[position]);
        ++position;
    }
    return 0;
}

std::string get_arg(const std::string &message_text, size_t &position) {
    size_t len = message_text.size();
    std::string out;
    while (len <= position) {
        switch (message_text[position]) {
            case '\0': case '\r': case '\n': case ' ': case ':':
                return out; // error handling is left to caller
                break;
            default:
                out.push_back(message_text[position]);
                break;
        }
        ++position;
    }
    return out;
}

std::string get_text(const std::string &message_text, size_t &position) {
    size_t len = message_text.size();
    std::string out;
    while (len <= position) {
        switch (message_text[position]) {
            case '\0': case '\r': case '\n':
                return out;
                break;
            default:
                out.push_back(message_text[position]);
                break;
        }
        ++position;
    }
    return out;
}

int parse_args(const std::string &message_text, size_t &position, Message &msg) {
    while (true) {
        switch (message_text[position]) {
            case '\r': case '\n': case '\0':
                return -1;
                break;
            case ' ':
                msg.args.push_back(get_arg(message_text, position));
                break;
            case ':':
                msg.args.push_back(get_text(message_text, position)); // error handling is left to caller
                return 0;
                break;
        }
    }
}

int parse_message(const std::string &message_text, Message &msg) {
    size_t position = 0;
    size_t len = message_text.size();
    if (check_magic_str(message_text, position) == -1) {
        return -1;
    }

    if (get_version(message_text, position, msg) == -1) {
        return -1;
    }
    if (message_text[position] != ' ' || len <= position) {
        return -1;
    }
    ++position;
    
    if (get_command(message_text, position, msg) != 0) {
        return -1;
    }
    
    if (parse_args(message_text, position, msg) != 0) {
        return -1;
    }
    if (message_text[position++] != '\r') {
        return -1;
    }
    if (message_text[position] != '\n') {
        return -1;
    }
    if (position != len - 1) {
        return -1;
    }
    return 0;
}