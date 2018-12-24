/**
 * Distributed under the MIT License (MIT)
 * Copyright (c) 2018 icekylin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DM_MEMORY_BUFFER_H
#define DM_MEMORY_BUFFER_H
#include <string>
#include <iostream>
#include <cstring>

namespace dm {
class basic_memory_buffer {
public:
    basic_memory_buffer() :pos_(0) {
        std::memset(buffer_, 0, 2048);
    }

    void clear() {
        std::memset(buffer_, 0, 2048);
        pos_ = 0;
    }

    const char *data() const {
        return buffer_;
    }

    char *get_buffer() {
        return buffer_;
    }

    const int size() const {
        return pos_;
    }

    int append(const char *src, int len) {
        std::memcpy(buffer_ + pos_, src, len);
        pos_ += len;
        return len;
    }

    inline basic_memory_buffer &operator<<(const char &ch) {
        buffer_[pos_] = ch;
        ++pos_;
        return *this;
    }

    inline basic_memory_buffer &operator<<(const char *s) {
        int len = strlen(s);
        std::memcpy(buffer_ + pos_, s, len);
        pos_ += len;
        return *this;
    }

    inline basic_memory_buffer &input_bool(bool b) {
        const char *buf[2] = {"true", "false"};
        int type = b ? 0 : 1;
        *this<<buf[type];
    }

    inline basic_memory_buffer &operator<<(const std::string s) {
        std::memcpy(buffer_ + pos_, s.c_str(), s.size());
        pos_ += s.size();
        return *this;
    }

    inline basic_memory_buffer &operator<<(basic_memory_buffer &buf) {
        std::memcpy(buffer_ + pos_, buf.data(), buf.size());
        pos_ += buf.size();
        return *this;
    }
private:
    char buffer_[2048];
    int  pos_;
};

}	  // namespace dm
#endif	// DM_MEMORY_BUFFER_H