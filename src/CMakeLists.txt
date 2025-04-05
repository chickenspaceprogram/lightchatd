#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16)

add_subdirectory(tests)
add_library(sockets socket.cpp socket.hpp)
add_library(events event.hpp listener.cpp listener.hpp)

