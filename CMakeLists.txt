#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16)
project(
    chatservd
    VERSION 0.1
    DESCRIPTION "A multithreaded chat server daemon, written in C++ with the libevent library."
    LANGUAGES CXX
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-levent_core") # bruteforcing this because for some unknown reason fedora's libevent-devel package doesn't install the cmake config files needed for find_package to work
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined -fsanitize=leak") # comment this out if you want to check for leaks with valgrind
endif()

include(CTest)
add_subdirectory(src)
