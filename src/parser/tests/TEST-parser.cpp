/*
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

#include "../parser.hpp"
#include <assert.h>

int main(void) {
    return 0;
    Message message;
    std::string yeet = "LIGHTCHAT/0.0 CONNECT username";
    assert(parse_message(yeet, message) == 0);
    assert(message.command == "CONNECT");
    assert(message.major_version == 0);
    assert(message.minor_version == 0);
    std::vector<std::string> thing;
    thing.push_back("username");
    assert(message.args == thing);

    // add more tests here lol
}