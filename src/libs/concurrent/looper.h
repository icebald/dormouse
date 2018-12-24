//
// Created by icekylin on 18-12-20.
//

#ifndef DM_LOOPER_H
#define DM_LOOPER_H

#include <memory>
#include <mutex>
#include "concurrent/message_queue.h"
#include "concurrent/message.h"
#include "log/log.h"

namespace dm {
namespace concurrent {

class looper {
public:
    typedef std::shared_ptr<looper> looper_ptr;
    typedef message_queue<msg_ptr> msg_queue;
protected:
    looper() {
        msgs_ = new msg_queue();
    }

public:
    ~looper() {
        if (nullptr != msgs_) {
            delete msgs_;
            msgs_ = nullptr;
        }
    }

    static void prepare(bool quit_allowed) {
        if (my_looper_) return;
        my_looper_.reset(new looper);
    }

    static void prepare_main_looper() {
        prepare(false);
        std::lock_guard<std::mutex> lock(main_mutex_);
        if (!main_looper_) {
            main_looper_ = my_looper_;
        }
    }

    static looper &my_looper() {
        return *my_looper_;
    }

    static looper &main_looper() {
        return *main_looper_;
    }

    static bool is_main() {
        return main_looper_ == my_looper_;
    }

    static int loop() {
        while (true) {
            msg_ptr msg = my_looper_->msgs_->pop();
            try {
                msg->_M_run();
            } catch (std::system_error e) {
                if (e.code().value() == 0) {
                    break;
                }
                return e.code().value();
            }
        }
        return 0;
    }
    msg_queue *get_queue() {
        return msgs_;
    }

    void quit() {
        msgs_->quit();
    }
private:
    static thread_local looper_ptr my_looper_;
    static looper_ptr main_looper_;
    static std::mutex main_mutex_;

private:
    msg_queue *msgs_;
};

looper::looper_ptr looper::main_looper_;
thread_local looper::looper_ptr looper::my_looper_;
std::mutex looper::main_mutex_;

}
}
#endif //DM_LOOPER_H
