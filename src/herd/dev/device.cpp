#include "device.h"
#include "adb/adbhelper.h"
#include <QDebug>
#include <QStringList>
#include <thread>
#include <QProcess>

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

void device::ReadyRead() {
    qDebug()<<QString::fromLocal8Bit(mP->readAll());
}

void device::run() {
    QString cmd = QString(LOGCAT).arg(mSerialNumber).arg(mProgress.at(mCurrentProgress).pid);
    mP = new QProcess();
    mP->start(cmd);
    mP->waitForStarted();
    mIsRun = true;
    while (mIsRun) {
        mP->waitForReadyRead(1000);
        qDebug()<<QString::fromLocal8Bit(mP->readAll());
    }
}

void device::stop() {
    mIsRun = false;
}

void device::catLogcat() {
    start();
}
