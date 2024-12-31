// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <stddef.h>

class DataBuf {
    public:
        DataBuf() : len(0), buf(NULL) {}
        DataBuf(size_t len);

        ~DataBuf();

        void setSize(size_t len);
        inline size_t getSize(void) { return len; }

        char *buf; // public fields are not idiomatic in C++ but oh well
    private:
        size_t len;
};
