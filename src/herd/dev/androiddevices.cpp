#include "ui/mainwindow.h"
#include "androiddevices.h"
#include "device.h"
#include "adb/adbhelper.h"
#include "ui/events.h"

#include <QDebug>
#include <thread>
#include <QEvent>
#include <QApplication>

AndroidDevices::AndroidDevices(MainWindow *window) : mHwnd(window)
{

}

void AndroidDevices::buildDevInfo(const QString &str) {
    QStringList sl =str.split(" ");
    if (sl.count() <= 0) return;
    dev_ptr dev(new device(sl.at(0)));
    for (int i = 1; i < sl.count(); i++) {
        if (sl.at(i).indexOf("model:") != -1) {
            QStringList strs = sl.at(i).split(":");
            if (strs.count() < 2) continue;

            dev->setModel(strs.at(1));
        } else if (sl.at(i).indexOf("product:") != -1) {
            QStringList strs = sl.at(i).split(":");
            if (strs.count() < 2) continue;
            dev->setProduct(strs.at(1));
        } else if (sl.at(i).indexOf("device:") != -1) {
            QStringList strs = sl.at(i).split(":");
            if (strs.count() < 2) continue;
            dev->setDevice(strs.at(1));
        }
    }
    mDevices.push_back(dev);
}

void AndroidDevices::buildDevice(const QString &str) {
    QStringList sl =str.split("\n");
    for (int i=0; i < sl.count(); i++) {
        if (sl.at(i).indexOf("device") == -1 || sl.at(i).indexOf("attached") != -1) {
            continue;
        }
        buildDevInfo(sl.at(i));
    }

    QApplication::postEvent(mHwnd, new DeviceEvent(mDevices));
}

void AndroidDevices::run() {
    QString version = AdbHelper::excue(GET_VERSION);
    QApplication::postEvent(mHwnd, new VersionEvent(version));

    while (true) {
        mDevices.clear();
        buildDevice(AdbHelper::excue(LIST_DEVICE));
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

void AndroidDevices::setSelectDev(int index) {
    mSelectDev = index;
}

void AndroidDevices::setPidSelect(int row, int index) {
    setSelectDev(row);
    mDevices.at(row)->setCurrentProgress(index);
    QApplication::postEvent(mHwnd, new SetSelectDevEvent(mDevices.at(row)));
}