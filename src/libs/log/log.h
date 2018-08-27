/**
 * Distributed under the MIT License (MIT)
 * Copyright (c) 2016 icekylin
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

/** pattern
 * %n  --  logger name
 * %l  --  short level str
 * %t  --  thread id
 * %c  --  Mon Aug 2716:22:42
 * %C  --  year 2 bits
 * %D  --  Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
 * %Y  --  year 4 bits
 * %m  --  month
 * %d  --  day of month 1-31
 * %H  --  hours in 24 format 0-23
 * %I  --  hours in 12 format 1-12
 * %M  --  minutes 0-59
 * %S  --  seconds 0-59
 * %e  --  milliseconds
 * %P  --  process id
 * %T  --  2018/08/27 16:23:17
 * the default pattern is "[%l %T.%e %t]", e.
 *      loge("HELLO") outputs is  [E 2018/08/27 16:29:43.355 388936] HELLO
 */
#ifndef DM_HEADER_H
#define DM_HEADER_H
#include <atomic>
#include <iostream>
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <exception>
#include <mutex>
#include <system_error>
#include <pthread.h>
#include <thread>
#include <unistd.h>

#include "utils/file_helper.h"
#include "utils/noncopyable.h"
#include "utils/format.h"
#include "utils/memory_buffer.h"
#include "utils/time.h"
#include "utils/null_mutex.h"
#include "utils/colors.h"

#define DEFAULT_LOG_NAME "default"
#define DEFAULT_LOG_PATTERN "[%l %T.%e %t]"

namespace dm {

namespace level {
enum level_enum {
    trace = 0,
    info = 1,
    debug = 2,
    warn = 3,
    err = 4
};	  // enum level_enum

const char *short_level_str[] = {"T", "I", "D", "W", "E"};

}	  // namespace level

using level_t = std::atomic<int>;
class log_ex : public std::runtime_error {
public:
    explicit log_ex(const std::string &msg) : runtime_error(msg) { }
    log_ex(const std::string &msg, int last_errno) : runtime_error(msg) , last_errno_(last_errno) { }
    const char *what() const noexcept override {
        if (last_errno_) {
            std::error_code ec(last_errno_, std::system_category());
            return ec.message().append(":").append(runtime_error::what()).c_str();
        } else {
            return runtime_error::what();
        }
    }
private:
    int last_errno_{0};
};

namespace details {
struct log_msg {
    log_msg() = default;
    log_msg(const log_msg &other) = delete;
    log_msg(log_msg &&other) = delete;
    log_msg &operator=(log_msg &&other) = delete;

    explicit log_msg(const char *name, dm::level::level_enum lvl)
        : raw_(), level_(lvl), name_(name) {
        time_point_ = log_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(time_point_);
        tm_time_ = dm::details::localtime(tt);

        pthread_threadid_np(nullptr, &thread_id_);
    }

    ~log_msg() {
        raw_.clear();
    }

    dm::basic_memory_buffer raw_;
    dm::level::level_enum level_;
    std::string name_;
    uint64_t thread_id_;
    log_clock::time_point time_point_;
    std::tm tm_time_;
};

template <char name>
struct flag_formatter {
    static void format(details::log_msg &msg) {}
};

template <>
struct flag_formatter<'n'> {
    static void format(details::log_msg &msg) {
        msg.raw_ << msg.name_;
    }
};

template <>
struct flag_formatter<'l'> {
    static void format(details::log_msg &msg) {
        msg.raw_ << level::short_level_str[msg.level_];
    }
};

template <>
struct flag_formatter<'t'> {
    static void format(details::log_msg &msg) {
        fmt::details::internal::format_decimal(msg.raw_, msg.thread_id_);
    }
};

template <>
struct flag_formatter<'c'> {
    static void format(details::log_msg &msg) {
        msg.raw_ << dm::get_day(msg.tm_time_.tm_wday);
        msg.raw_ << ' ';
        msg.raw_ << dm::get_month(msg.tm_time_.tm_mon);
        msg.raw_ << ' ';
        fmt::details::internal::format_decimal(msg.raw_, msg.tm_time_.tm_mday);

        dm::fmt_helper::pad2(msg.tm_time_.tm_hour, msg.raw_);
        msg.raw_ << ":";
        fmt_helper::pad2(msg.tm_time_.tm_min, msg.raw_);
        msg.raw_ << ':';
        fmt_helper::pad2(msg.tm_time_.tm_sec, msg.raw_);
        msg.raw_ << ' ';
        fmt::details::internal::format_decimal(msg.raw_, msg.tm_time_.tm_year + 1900);
    }
};

// year - 2 digit
template <>
struct flag_formatter<'C'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_year % 100, msg.raw_);
}
};

// Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
template <>
struct flag_formatter<'D'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_mon + 1, msg.raw_);
    msg.raw_ << '/';
    fmt_helper::pad2(msg.tm_time_.tm_mday, msg.raw_);
    msg.raw_ << '/';
    fmt_helper::pad2(msg.tm_time_.tm_year % 100, msg.raw_);
}
};

// year 4bit
template <>
struct flag_formatter<'Y'> {
static void format(details::log_msg &msg) {
    fmt_helper::append_int(msg.tm_time_.tm_year + 1900, msg.raw_);
}
};

template <>
struct flag_formatter<'m'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_mon + 1, msg.raw_);
}
};

// day of month 1-31
template <>
struct flag_formatter<'d'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_mday, msg.raw_);
}
};

// hours in 24 format 0-23
template <>
struct flag_formatter<'H'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_hour, msg.raw_);
}
};

// hours in 12 format 1-12
template <>
struct flag_formatter<'I'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(dm::to12h(msg.tm_time_), msg.raw_);
}
};

// minutes 0-59
template <>
struct flag_formatter<'M'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_min, msg.raw_);
}
};

// seconds 0-59
template <>
struct flag_formatter<'S'> {
static void format(details::log_msg &msg) {
    fmt_helper::pad2(msg.tm_time_.tm_sec, msg.raw_);
}
};

// milliseconds
template <>
struct flag_formatter<'e'> {
static void format(details::log_msg &msg) {
    auto duration = msg.time_point_.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    fmt_helper::pad3(static_cast<int>(millis), msg.raw_);
}
};

template <>
struct flag_formatter<'P'> {
static void format(details::log_msg &msg) {
    fmt_helper::append_int(static_cast<int>(::getpid()), msg.raw_);
}
};

template <>
struct flag_formatter<'T'> {
static void format(details::log_msg &msg) {
    fmt_helper::append_int(msg.tm_time_.tm_year + 1900, msg.raw_);
    msg.raw_ << '/';
    fmt_helper::pad2(msg.tm_time_.tm_mon + 1, msg.raw_);
    msg.raw_ << '/';
    fmt_helper::pad2(msg.tm_time_.tm_mday, msg.raw_);
    msg.raw_ << ' ';
    fmt_helper::pad2(msg.tm_time_.tm_hour, msg.raw_);
    msg.raw_ << ':';
    fmt_helper::pad2(msg.tm_time_.tm_min, msg.raw_);
    msg.raw_ << ':';
    fmt_helper::pad2(msg.tm_time_.tm_sec, msg.raw_);
}
};

struct formatter {
static void compile_pattern(details::log_msg &msg, const std::string &pattern) {
    for (auto it = pattern.begin(); it != pattern.end(); ++it) {
        if (*it == '%') {
            if (++it != pattern.end() && handle_flag(msg, *it)) {
                continue;
            }
        }
        msg.raw_ << *it;
    }
}

static bool handle_flag(details::log_msg &msg, char flag) {
    switch (flag) {
        case 'n': {
            flag_formatter<'n'>::format(msg);
            return true;
        }
        case 'l': {
            flag_formatter<'l'>::format(msg);
            return true;
        }
        case 't': {
            flag_formatter<'t'>::format(msg);
            return true;
        }
        case 'c': {
            flag_formatter<'c'>::format(msg);
            return true;
        }
        case 'C': {
            flag_formatter<'C'>::format(msg);
            return true;
        }
        case 'D': {
            flag_formatter<'D'>::format(msg);
            return true;
        }
        case 'Y': {
            flag_formatter<'Y'>::format(msg);
            return true;
        }
        case 'm': {
            flag_formatter<'m'>::format(msg);
            return true;
        }
        case 'H': {
            flag_formatter<'H'>::format(msg);
            return true;
        }
        case 'd': {
            flag_formatter<'d'>::format(msg);
            return true;
        }
        case 'I': {
            flag_formatter<'I'>::format(msg);
            return true;
        }
        case 'M': {
            flag_formatter<'M'>::format(msg);
            return true;
        }
        case 'S': {
            flag_formatter<'S'>::format(msg);
            return true;
        }
        case 'e': {
            flag_formatter<'e'>::format(msg);
            return true;
        }
        case 'P': {
            flag_formatter<'P'>::format(msg);
            return true;
        }
        case 'T': {
            flag_formatter<'T'>::format(msg);
            return true;
        }
        default: {
            throw dm_error("invalid format param.");
        }
    }
    return true;
}
};

}	  // namespace details

namespace sinks {
struct sink {
    virtual void log(const details::log_msg &msg) = 0;
    virtual void flush() = 0;
};

template <typename Mutex>
class base_sink : public sink {
public:
    base_sink() : mutex_() {}
    base_sink(const base_sink &) = delete;
    base_sink &operator=(const base_sink &) = delete;
    void log(const details::log_msg &msg) final override {
        std::lock_guard<Mutex> lock(mutex_);
        sink_it_(msg);
    }
    void flush() final override {
        std::lock_guard<Mutex> lock(mutex_);
        flush_();
    }
protected:
    virtual void sink_it_(const details::log_msg &msg) = 0;
    virtual void flush_() = 0;
    Mutex mutex_;
};

template <typename _Mutex>
class ascii_color_sink : public base_sink<_Mutex> {
public:
    ascii_color_sink(FILE *fp) : mutex_(), fp_(fp) {
        should_do_colors_ = os::in_terminal(fp) &&
                            os::is_color_terminal();
    }

    virtual void sink_it_(const details::log_msg &msg) {
        static const std::string level_color_str[] = {
            dm::ascii_colors_str[dm::ascii_color::white],
            dm::ascii_colors_str[dm::ascii_color::green],
            dm::ascii_colors_str[cyan],
            dm::ascii_colors_str[dm::ascii_color::yellow] + dm::ascii_colors_str[bold],
            dm::ascii_colors_str[red] + dm::ascii_colors_str[bold]
        };
        
        dm::vprint_ascii_color(level_color_str[msg.level_], msg.raw_.data());
    }

    virtual void flush_() {
        std::fflush(fp_);
    }

private:
    _Mutex mutex_;
    bool should_do_colors_;
    FILE *fp_;
};
using ascii_color_sink_st = ascii_color_sink<dm::null_mutex>;
using ascii_color_sink_mt = ascii_color_sink<std::mutex>;

template <class _Mutex>
class basic_file_sink : public base_sink<_Mutex> {
public:
    explicit basic_file_sink(const char *filename, bool truncate = false) {
        fh_.open(filename, truncate);
    }

protected:
    virtual void sink_it_(const details::log_msg &msg) override {
        fh_.write(msg.raw_.data(), msg.raw_.size());
    }

    void flush_() override{
        fh_.flush();
    }

private:
    dm::details::file_helper fh_;
};

using basic_file_sink_st = basic_file_sink<dm::null_mutex>;

}	  // namespace sinks

using sink_ptr = std::shared_ptr<dm::sinks::sink>;
using init_list = std::initializer_list<sink_ptr>;
using sink_array = std::vector<sink_ptr>;
using sink_array_it = sink_array::iterator;
using sink_map = std::map<std::string, sink_array>;
using sink_map_it = sink_map::iterator;

class logger {
public:
    logger() : can_log_(false) {
    }
    logger(const std::string &pattern, const char *name,
           sink_array &array, dm::level::level_enum lvl) 
        : pattern_(pattern), name_(name), sinks_(array), can_log_(true), level_(lvl) {}
    logger &operator()(dm::level::level_enum lvl) {
        return *this;
    }

    template <typename ...Args>
    void operator()(const char *fmt, Args ...args) {
        if (!can_log_) return;
        details::log_msg msg(name_.c_str(), level_);
        details::formatter::compile_pattern(msg, pattern_);
        msg.raw_ << ' ';
        dm::fmt::format_to(msg.raw_, fmt, args...);
        std::for_each(sinks_.begin(), sinks_.end(), [&](const sink_ptr sink) {
            sink->log(msg);
        });
    }

    template <typename ...Args>
    void operator()(const char *fmt, basic_memory_buffer &buffer, Args ...args) {
        if (!can_log_) return;
        details::log_msg msg(name_.c_str(), level_);
        details::formatter::compile_pattern(msg, pattern_);
        msg.raw_ << ' ';
        msg.raw_ << buffer;
        dm::fmt::format_to(msg.raw_, fmt, args...);
        std::for_each(sinks_.begin(), sinks_.end(), [&](const sink_ptr sink) {
            sink->log(msg);
        });
    }

    logger &operator<<(const char *val) {
        if (!can_log_) return *this;
        std::cout<<val;
        return *this;
    }
private:
    std::string name_;
    std::string pattern_;
    bool can_log_;
    sink_array sinks_;
    dm::level::level_enum level_;
};

class logger_helper : public dm::noncopyable {
public:
    friend dm::level::level_enum get_level();
    friend void set_level(dm::level::level_enum lvl);
    friend void enable_default(bool enable);
    friend void set_pattern(const char *name, const char *pattern);
    friend void create(const char *log_name, const char *pattern, dm::init_list list);

    logger operator()(const char *name, dm::level::level_enum lvl) {
        std::lock_guard<std::mutex> lock(mutex_);
        sink_map_it it = sinks_.find(name);
        if (it == sinks_.end()) return null_log_;

        if (should_log(lvl)) return logger(get_pattern(name), name, it->second, lvl);
        return null_log_;
    }

    static logger_helper &instance() {
        static logger_helper lh;
        return lh;
    }

    void remove_log(const char *name) {
        std::lock_guard<std::mutex> lock(mutex_);
        sinks_.erase(name);
    }
protected:
    void enable_default(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (enable) {
            dm::sink_ptr sink(new dm::sinks::ascii_color_sink_mt(stdout));
            create(DEFAULT_LOG_NAME, {sink});
        } else {
            remove(DEFAULT_LOG_NAME);
        }
    }

    dm::level::level_enum get_level() {
        std::lock_guard<std::mutex> lock(mutex_);
        return static_cast<dm::level::level_enum>(level_.load(std::memory_order_relaxed));
    }

    void set_level(dm::level::level_enum lvl) {
        std::lock_guard<std::mutex> lock(mutex_);
        level_.store(lvl);
    }

    void create(const char *name, init_list list) {
        create(name, DEFAULT_LOG_PATTERN, list);
    }

    void create(const char *name, const char *pattern, init_list list) {
        set_pattern(name, pattern);
        sink_array sa(list);
        sink_map_it it = sinks_.find(name);
        if (it != sinks_.end()) {
            sinks_.erase(it);
        }
        sinks_.insert(std::make_pair(name, sa));
    }

    void remove(const char *name) {
        std::lock_guard<std::mutex> lock(mutex_);
        sinks_.erase(name);
    }

    logger_helper() : level_(dm::level::trace), null_log_() {
        dm::sink_ptr locak_sink(new dm::sinks::ascii_color_sink_mt(stdout));
        create(DEFAULT_LOG_NAME, {locak_sink});
    }

    bool should_log(dm::level::level_enum msg_level) const {
        return msg_level >= level_.load(std::memory_order_relaxed);
    }

    void set_pattern(const std::string &name, const std::string &pattern) {
        std::lock_guard<std::mutex> lock(mutex_);
        pattern_map_it pattern_it = patterns_.find(name);
        if (pattern_it == patterns_.end()) {
            patterns_.insert(std::make_pair(name, pattern));
            return;
        }
        pattern_it->second = pattern;
    }

    const std::string &get_pattern(const char *name) {
        return patterns_.find(name)->second;
    }

private:
    typedef std::map<std::string, std::string> pattern_map;
    typedef pattern_map::iterator pattern_map_it;

    sink_map sinks_;
    dm::level_t level_;
    logger null_log_;
    std::mutex mutex_;
    pattern_map patterns_;
};

extern dm::level::level_enum get_level() {
    return logger_helper::instance().get_level();
}

extern void set_level(dm::level::level_enum lvl) {
    logger_helper::instance().set_level(lvl);
}

extern void enable_default(bool enable) {
    logger_helper::instance().enable_default(enable);
}

extern void set_pattern(const char *name, const char *pattern) {
    logger_helper::instance().set_pattern(name, pattern);
}

extern void create(const char *log_name, const char *pattern, dm::init_list list) {
    logger_helper::instance().create(log_name, pattern, list);
}

extern void log_init() {
}
}	  // namespace dm

#define log(logger_name, lvl) dm::logger_helper::instance()(logger_name, lvl)

#define _logt(_name_) log(_name_, dm::level::trace)
#define _logi(_name_) log(_name_, dm::level::info)
#define _logd(_name_) log(_name_, dm::level::debug)
#define _logw(_name_) log(_name_, dm::level::warn)
#define _loge(_name_) log(_name_, dm::level::err)

#define logt(name, ...) _logt(name)(__VA_ARGS__)
#define logi(name, ...) _logi(name)(__VA_ARGS__)
#define logd(name, ...) _logd(name)(__VA_ARGS__)
#define logw(name, ...) _logw(name)(__VA_ARGS__)
#define loge(name, ...) _loge(name)(__VA_ARGS__)

#define dlogt _logt(DEFAULT_LOG_NAME)
#define dlogi _logi(DEFAULT_LOG_NAME)
#define dlogd _logd(DEFAULT_LOG_NAME)
#define dlogw _logw(DEFAULT_LOG_NAME)
#define dloge _loge(DEFAULT_LOG_NAME)

#define LOG_GOTO_FMT "[%s:%d] "

// TODO: fix it two level.
#define dlogi_goto(_fmt, _gotofmt, _file, _line, ...)\
do {\
    dm::basic_memory_buffer _buf;\
    dm::fmt::format_to(_buf, _gotofmt, _file, _line);\
    dlogi(_fmt, _buf, __VA_ARGS__);\
} while (0)

#ifndef NDEBUG
#define dlog(fmt, ...)\
dlogi_goto(fmt, LOG_GOTO_FMT, __FILENAME__, __LINE__, __VA_ARGS__)

namespace dm {
#define LOG_TRACE_FMT "[%s/%s:%d] "
#define LOG_TRACE_FMT_LEAVE "[%s:%s] "
#define FUNC_ENTRY    "ENTRY"
#define FUNC_LEAVE    "leave"
struct log_trace {
    log_trace(const char *file, const char *func, int line) : file_(file), func_(func), line_(line) {
        dm::basic_memory_buffer buf;
        dm::fmt::format_to(buf, LOG_TRACE_FMT, file, func, line);
        dlogi("%s\n", buf, FUNC_ENTRY);
    }

    ~log_trace() {
        dm::basic_memory_buffer buf;
        dm::fmt::format_to(buf, LOG_TRACE_FMT_LEAVE, file_.c_str(), func_.c_str());
        dlogi("%s\n", buf, FUNC_LEAVE);
    }

    std::string file_;
    std::string func_;
    int line_;
};
}

#define LOG_FUNC_TRACE()\
dm::log_trace __lt(__FILENAME__, __FUNCTION__, __LINE__)
#else
#define dlog(fmt, ...)
#define LOG_FUNC_TRACE()
#endif

#endif	// DM_HEADER_H