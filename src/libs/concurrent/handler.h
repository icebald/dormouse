//
// Created by icekylin on 18-12-20.
//

#ifndef DM_HANDLER_H
#define DM_HANDLER_H

#include <memory>
#include "looper.h"

namespace dm {
namespace concurrent {
class handler {
public:
    handler() : looper_(looper::my_looper()) {
    }

    void stop() {
        looper_.quit();
    }

    template <typename _Func, typename ...Args>
    msg_ptr post(_Func&& __f, Args&&... __args) {
        return looper_.post(__f, __args...);
    }

    template <typename _Func, typename _Rep, typename _Period, typename ...Args>
    msg_ptr post_delayed(_Func&& __f, const std::chrono::duration<_Rep, _Period> &time, Args&&... __args) {
        return looper_.post_delayed(__f, time, __args...);
    }

    void remove(msg_ptr msg) {
        looper_.remove(msg);
    }
private:
    looper &looper_;
};
typedef std::shared_ptr<handler> handler_ptr;
}
}
#endif //DM_HANDLER_H
