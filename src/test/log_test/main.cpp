
#include <iostream>
#include <functional>
#include "concurrent/looper.h"
#include "log/log.h"
#include "concurrent/dispatch.h"
#include "net/net_dispatch.h"

int main(int argc, char *argv[]) {
    dm::concurrent::looper::prepare_main_looper();
    dm::concurrent::looper::main_looper().post_delayed([]()-> void {LOG_FUNC_TRACE();}, std::chrono::seconds(1));
    return dm::concurrent::looper::loop();
}
