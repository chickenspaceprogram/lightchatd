// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <stddef.h>

class DataBuf {
    public:
        DataBuf() : buf(NULL), len(0) {}
        DataBuf(size_t len);
        DataBuf(DataBuf &dbuf) = delete; // copying is expensive and i want compiler errors if i try to do it
        DataBuf(DataBuf &&dbuf) : buf(dbuf.buf), len(dbuf.len) {} // moving is fine, this shouldn't cause use-after-frees since ownership is transferred

        ~DataBuf();

        void setSize(size_t len);
        inline size_t getSize(void) { return len; }

        char *buf; // public fields are not idiomatic in C++ but oh well
    private:
        size_t len;
};
