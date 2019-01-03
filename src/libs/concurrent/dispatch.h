//
// Created by icekylin on 18-12-20.
//

#ifndef DM_DISPATCH_H
#define DM_DISPATCH_H
#include <thread>
#include <memory>
#include "utils/invoker.h"
#include "handler.h"

namespace dm {
namespace concurrent {
class dispatch {
public:
    dispatch() {
    }

    virtual ~dispatch() {
        if (thread_) {
            thread_.reset();
        }

        if (handler_) {
            handler_.reset();
        }
    }

    void start() {
        thread_.reset(new std::thread(&dispatch::run, this));
    }

    void stop() {
        handler_->stop();
        post([]() -> void {throw std::system_error(std::error_code()); });
        thread_->join();
    }

    template <typename _Func, typename ...Args>
    msg_ptr post(_Func&& __f, Args&&... __args) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!handler_ && not_init_.wait_for(mutex_, std::chrono::seconds(2)) == std::cv_status::timeout) {
            throw dm::dm_error("dispatch not start.");
        }
        return handler_->post(__f, __args...);
    }

    template <typename _Func, typename _Rep, typename _Period, typename ...Args>
    msg_ptr post_delayed(_Func&& __f, const std::chrono::duration<_Rep, _Period> &time, Args&&... __args) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!handler_ && not_init_.wait_for(mutex_, std::chrono::seconds(2)) == std::cv_status::timeout) {
            throw dm::dm_error("dispatch not start.");
        }
        if (time.count() == 0) {
            return handler_->post(__f, __args...);
        }
        return handler_->post_delayed(__f, time, __args...);
    }

    void remove(msg_ptr msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!handler_) {
            not_init_.wait(mutex_);
        }
        handler_->remove(msg);
    }
protected:

    void run() {
        looper::prepare(true);
        handler_.reset(new handler());
        not_init_.notify_one();
        looper::loop();
    }
private:
    std::mutex mutex_;
    std::condition_variable_any not_init_;
    handler_ptr handler_;
    std::shared_ptr<std::thread> thread_;
};
}
}
#endif //DM_DISPATCH_H
