#ifndef ADBHELPER_H
#define ADBHELPER_H

#include <QString>

const QString LIST_DEVICE = "adb -d devices -l";
const QString LIST_JDWP = "adb -s %1 jdwp &";
const QString GET_PROGRESS_NAME = "adb -s %1 shell ps |grep %2";
const QString LOGCAT = "adb -s %1 logcat |grep %2";
const QString GET_VERSION = "adb version";

struct AdbHelper {
    static QString excue(const QString &cmd);
};

#endif // ADBHELPER_H
