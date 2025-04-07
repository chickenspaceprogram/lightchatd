cmake_minimum_required(VERSION 3.16)

add_library(fd-io-replacements sockfuncs.cpp sockfuncs.hpp)

add_executable(test-fd-io-replacements test-sockfuncs.cpp)
target_link_libraries(test-fd-io-replacements fd-io-replacements)

add_test(
    NAME test-fd-io-replacements
    COMMAND $<TARGET_FILE:test-fd-io-replacements>
)

add_executable(test-connection connection.cpp)

target_link_libraries(test-connection fd-io-replacements sockets)
add_test(
    NAME test-connection
    COMMAND $<TARGET_FILE:test-connection>
)

