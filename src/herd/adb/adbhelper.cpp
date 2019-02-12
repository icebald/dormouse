#include "adbhelper.h"
#include <QProcess>

QString AdbHelper::excue(const QString &cmd, int time) {
    QProcess p(0);
    p.start(cmd);
    p.waitForStarted();
    p.waitForFinished(time);
    p.finished(0);
    return QString::fromLocal8Bit(p.readAllStandardOutput());
}
