//
// Created by icekylin on 18-12-20.
//

#ifndef DM_MESSAGE_QUEUE_H
#define DM_MESSAGE_QUEUE_H
#include <list>
#include <condition_variable>
#include <cstdint>
#include <map>
#include "log/log.h"

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

    void remove(const T &value) {
        std::lock_guard<std::mutex> locker(mutex_);
        for (typename std::list<T>::iterator it = list_.begin(); it != list_.end(); ++it) {
            if (*it == value) {
                list_.erase(it);
                break;
            }
        }
    }

    void push(const T &value) {
        std::lock_guard<std::mutex> locker(mutex_);
        while (full()) {
            not_full_.wait(mutex_);
        }
        list_.push_back(value);
        not_empty_.notify_one();
    }

    void push_delayed(const T &value) {
        std::lock_guard<std::mutex> locker(mutex_);
        while (full()) {
            not_full_.wait(mutex_);
        }

    }

    T pop() {
        std::lock_guard<std::mutex> locker(mutex_);
        while (empty()) {
            auto start = std::chrono::high_resolution_clock::now();
            if (not_empty_.wait_for(mutex_, std::chrono::seconds(2)) == std::cv_status::timeout) {
                if (has_timeout()) {
                    return time_msgs_.begin()->second;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout<<duration.count() << std::endl;
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

protected:
    bool has_timeout() {
        return false;
    }
private:
    std::mutex mutex_;
    bool is_quiting_;
    std::map<std::uint64_t, T> time_msgs_;
    std::condition_variable_any not_empty_;
    std::condition_variable_any not_full_;
    std::list<T> list_;
};

}
}
#endif //DM_MESSAGE_QUEUE_H
