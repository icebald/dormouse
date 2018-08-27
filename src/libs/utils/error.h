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
#ifndef DM_ERROR_H
#define DM_ERROR_H
#include <system_error>
#include <sstream>
#include <string>

namespace dm {
class dm_error : public std::runtime_error {
public:
    explicit dm_error(const std::string &msg) : runtime_error(msg) { }
    dm_error(const std::string &msg, int last_errno)
        : runtime_error(msg), last_errno_(last_errno) {
    }
    const char *what() const noexcept override {
        if (last_errno_) {
            std::stringstream ss;
            std::error_code ec(last_errno_, std::system_category());
            ss << runtime_error::what() << " : " << ec.message();
            return ss.str().c_str();
        } else {
            return runtime_error::what();
        }
    }
private:
    int last_errno_{0};
};
}	  // namespace dm
#endif	// DM_ERROR_H