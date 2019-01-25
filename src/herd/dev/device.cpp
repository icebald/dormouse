#include "device.h"
#include "adb/adbhelper.h"
#include <QDebug>
#include <QStringList>

device::device(const QString &name) : mSerialNumber(name) {
    QString cmd = QString(LIST_JDWP).arg(name);
    QString pids = AdbHelper::excue(cmd);
    QStringList strList = pids.split("\n");
    addProgress(strList);
}

void device::addProgress(QStringList &pids) {
    for (int i = 0; i < pids.count(); ++i) {
        QString cmd = QString(GET_PROGRESS_NAME).arg(mSerialNumber).arg(pids.at(i));
        QString reset = AdbHelper::excue(cmd);
        QStringList nameList = reset.split(" ");
        QString name = nameList.at(nameList.count() - 1).trimmed();
        if (name.isEmpty()) continue;
        Progress progress(name, pids.at(i));
        mProgress.push_back(progress);
    }
}
