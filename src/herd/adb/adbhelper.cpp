#include "adbhelper.h"
#include <QProcess>

QString AdbHelper::excue(const QString &cmd) {
    QProcess p(0);
    p.start(cmd);
    p.waitForStarted();
    p.waitForFinished(2 * 1000);
    p.finished(0);
    return QString::fromLocal8Bit(p.readAllStandardOutput());
}
