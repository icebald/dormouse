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
#ifndef DM_TIME_H
#define DM_TIME_H
#include <ctime>
#include <string>
#include <chrono>

namespace dm {
namespace details {
static const std::string days[]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const std::string months[]{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
inline std::tm localtime(const std::time_t &time_tt) {
#ifdef _WIN32
    std::tm tm;
    localtime_s(&tm, &time_tt);
#else
    std::tm tm;
    localtime_r(&time_tt, &tm);
#endif
    return tm;
}
}	  // namespace details

inline std::tm localtime() {
    std::time_t now_t = time(nullptr);
    return details::localtime(now_t);
}

inline const std::string &get_day(int pos) {
    return details::days[pos];
}

inline const std::string &get_month(int pos) {
    return details::months[pos];
}

inline int to12h(const std::tm &t) {
    return t.tm_hour > 12 ? t.tm_hour - 12 : t.tm_hour;
}

using log_clock = std::chrono::system_clock;

}	  // namespace dm
#endif	// DM_TIME_H