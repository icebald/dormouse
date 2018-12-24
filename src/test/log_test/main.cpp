
#include <iostream>
#include <functional>
#include "concurrent/looper.h"
#include "log/log.h"
#include "concurrent/dispatch.h"
#include "net/net_dispatch.h"

class test {
public:
    void testFunc() {
        LOG_FUNC_TRACE();
    }
};
int main(int argc, char *argv[]) {
    dm::concurrent::looper::prepare_main_looper();
    dm::concurrent::dispatch dispatch;
    dispatch.start();
    test a;
    dispatch.post(std::bind(&test::testFunc, &a));
    dispatch.stop();
    return dm::concurrent::looper::loop();
}
