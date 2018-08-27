#ifndef NULL_MUTEX_H
#define NULL_MUTEX_H

namespace dm {
struct null_mutex {
    void lock() {}
    void unlock() {}
};

}	  // namespace dm
#endif	// NULL_MUTEX_H