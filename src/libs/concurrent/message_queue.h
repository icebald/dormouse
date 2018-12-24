//
// Created by icekylin on 18-12-20.
//

#ifndef DM_MESSAGE_QUEUE_H
#define DM_MESSAGE_QUEUE_H
#include <list>
#include <condition_variable>
#include <cstdint>

namespace dm {
namespace concurrent {

template <typename T>
class message_queue {
public:
    bool full() const{
        return list_.size() >= INT32_MAX;
    }

    bool empty() const{
        return list_.empty();
    }

    void push(const T &value) {
        std::lock_guard<std::mutex> locker(mutex_);
        while (full()) {
            not_full_.wait(mutex_);
        }
        list_.push_back(value);
        not_empty_.notify_one();
    }

    T pop() {
        std::lock_guard<std::mutex> locker(mutex_);
        while (empty()) {
            not_empty_.wait(mutex_);
        }
        T value = list_.front();
        list_.pop_front();
        not_full_.notify_one();
        return value;
    }

    void quit() {
        if (is_quiting_) return;
        is_quiting_ = true;
        std::lock_guard<std::mutex> lock(mutex_);
        list_.clear();
    }
private:
    std::mutex mutex_;
    bool is_quiting_;
    std::condition_variable_any not_empty_;
    std::condition_variable_any not_full_;
    std::list<T> list_;
};

}
}
#endif //DM_MESSAGE_QUEUE_H
