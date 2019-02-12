#ifndef ANDROIDDEVICES_H
#define ANDROIDDEVICES_H
#include <QThread>
#include <QList>
#include "device.h"

class MainWindow;
class AndroidDevices : public QThread
{
    Q_OBJECT
public:
    AndroidDevices(MainWindow *hwnd);
    void run() override;
    void stop();
    void buildDevice(const QString &str);
    void buildDevInfo(const QString &str);
    void logcatPid(QString pid);
    void setSelectDev(int index);
    void setPidSelect(int row, int index);
private:
    int mSelectDev;
    bool mIsRun;
    MainWindow *mHwnd;
    QList<dev_ptr> mDevices;
};

#endif // ANDROIDDEVICES_H
