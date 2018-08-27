
#include <iostream>
#include "log/log.h"

int main(int argc, char *argv[]) {
    dm::set_pattern(DEFAULT_LOG_NAME, "%T.%e");

    dloge("asdf %s %d tt\n", "HELLO", 123);
    int a = 123;
    dlogd("test d %d\n", a);
    dlogi("test short %hd\n", (short)123);
    dlogw("test float %f\n", 12.00101);
    dlogt("test ... %d\n", 443);
    dlog("test %s", "HELLO\n");

    dm::sink_ptr sink(new dm::sinks::basic_file_sink_st("test.log"));
    dm::create("test", "%T.%e", {sink});
    logt("test", "ss %s\n", "HELLO");
    return 0;
}
