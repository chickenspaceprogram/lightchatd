
# Copyright 2024, 2025 Athena Boose

# This file is part of lightchatd.

# lightchatd is free software: you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.

# lightchatd is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
# for more details.

# You should have received a copy of the GNU Affero General Public License
# along with lightchatd. If not, see <https://www.gnu.org/licenses/>. 

cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)
add_library(socket
    buffer.hpp
    buffer.cpp
    socket.hpp
    socket.cpp
)
