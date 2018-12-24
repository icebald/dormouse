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

    void post(msg_ptr msg) {
        looper_.get_queue()->push(msg);
    }
private:
    looper &looper_;
};
typedef std::shared_ptr<handler> handler_ptr;
}
}
#endif //DM_HANDLER_H
