#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.15)

add_library(sockwriter
    databuf.cpp
    databuf.hpp
    msgqueue.hpp
    msgqueue.cpp
    msgsender.hpp
    msgsender.cpp
)

add_executable(TEST-databuf tests/TEST-databuf.cpp)

add_test(
    NAME databuf
    COMMAND $<TARGET_FILE:TEST-databuf>
)
