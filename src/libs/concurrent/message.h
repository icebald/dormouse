//
// Created by icekylin on 18-12-20.
//

#ifndef DM_MESSAGE_H
#define DM_MESSAGE_H

#include <memory>
#include <atomic>

namespace dm {
namespace concurrent {

struct message {
    std::atomic_bool cancle_;

    virtual ~message() {}
    virtual void _M_run() = 0;

    void cancle() {
        cancle_.store(true, std::memory_order_relaxed);
    }

    bool is_cancle() {
        return cancle_.load(std::memory_order_relaxed);
    }
};

using msg_ptr = std::shared_ptr<message>;

template <typename _Callable>
struct msg_impl : public message {
    _Callable _M_func;
    ~msg_impl() {}

    msg_impl(_Callable&& __f) : _M_func(std::forward<_Callable>(__f)) { }

    virtual void _M_run() { _M_func(); }
};

template <typename _Callable, typename _Rep, typename _Period>
struct time_msg_impl : public msg_impl<_Callable> {
    time_msg_impl(_Callable&& __f, const std::chrono::duration<_Rep, _Period> &time)
    : msg_impl<_Callable>(std::forward<_Callable>(__f)), time_(time) {}

    const std::chrono::duration<_Rep, _Period> &time_;
};

template<typename _Callable>
static msg_ptr
make_message(_Callable&& __f) {
    using _Impl = msg_impl<_Callable>;
    return msg_ptr{new _Impl{std::forward<_Callable>(__f)}};
}

template<typename _Callable, typename _Rep, typename _Period>
static msg_ptr
make_message(_Callable&& __f, const std::chrono::duration<_Rep, _Period> &time) {
    using _Impl = time_msg_impl<_Callable, _Rep, _Period>;
    return msg_ptr{new _Impl{std::forward<_Callable>(__f), time}};
}

}
}
#endif //DM_MESSAGE_H
