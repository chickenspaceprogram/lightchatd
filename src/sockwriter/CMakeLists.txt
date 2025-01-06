#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15)

add_library(client-msg-handling
    databuf.cpp
    databuf.hpp
)

add_executable(TEST-databuf tests/TEST-safesock.cpp)

add_test(
    NAME databuf
    COMMAND $<TARGET_FILE:TEST-databuf>
)