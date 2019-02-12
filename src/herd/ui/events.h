//
// Created by icebear on 2019/1/25.
//

#ifndef HERD_EVENTS_H
#define HERD_EVENTS_H
#include <QEvent>

const QEvent::Type REFRESH_DEVICE = QEvent::Type(QEvent::User + 2);
class DeviceEvent : public QEvent
{
public:
    explicit DeviceEvent(QList<dev_ptr> dev) : QEvent(REFRESH_DEVICE), mDevs(dev) {}

    QList<dev_ptr> getDev() { return mDevs; }
private:
    QList<dev_ptr> mDevs;
};

const QEvent::Type ADB_VERSION = QEvent::Type(QEvent::User + 3);
class VersionEvent : public QEvent
{
public:
    explicit VersionEvent(QString version) : QEvent(ADB_VERSION), mVersion(version) {}

    QString getVersion() { return mVersion; }
private:
    QString mVersion;
};

const QEvent::Type SET_SELECT_DEV = QEvent::Type(QEvent::User + 4);
class SetSelectDevEvent : public QEvent {
public:
    explicit SetSelectDevEvent(dev_ptr dev) : QEvent(SET_SELECT_DEV), mDev(dev) {}

    dev_ptr getDev() { return mDev; }
private:
    dev_ptr mDev;
};

#endif //HERD_EVENTS_H
