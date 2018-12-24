//
// Created by icekylin on 18-12-20.
//

#ifndef DM_MESSAGE_H
#define DM_MESSAGE_H

#include <memory>
namespace dm {
namespace concurrent {

struct message {
    virtual ~message() {}
    virtual void _M_run() = 0;
};

using msg_ptr = std::shared_ptr<message>;

template <typename _Callable>
struct msg_impl : public message {
    _Callable _M_func;
    ~msg_impl() {}

    msg_impl(_Callable&& __f) : _M_func(std::forward<_Callable>(__f)) { }

    virtual void _M_run() { _M_func(); }
};

template<typename _Callable>
static msg_ptr
make_message(_Callable&& __f) {
    using _Impl = msg_impl<_Callable>;
    return msg_ptr{new _Impl{std::forward<_Callable>(__f)}};
}

}
}
#endif //DM_MESSAGE_H
