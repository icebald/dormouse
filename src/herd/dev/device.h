#ifndef DEVICE_H
#define DEVICE_H
#include <QString>
#include <QEvent>
#include <memory>
#include <QList>

struct Progress {
    QString name;
    QString pid;
    Progress(QString n, QString p) {
        name = n;
        pid = p;
    }
};

class device
{
public:
    explicit device(const QString &name);

    QString getSerialNumber() { return mSerialNumber; }
    void setModel(QString model) {
        mModel = model;
    }
    QString getModel() { return mModel; }
    void setProduct(QString product) {
        mProduct = product;
    }
    QString getProduct() { return mProduct; }
    void setDevice(QString device) {
        mDevice = device;
    }
    QString getDevice() { return mDevice; }

    QList<Progress> &getProgress() { return mProgress; }
    void setCurrentProgress(int index) { mCurrentProgress = index; }
    const Progress &getCurrentProgress() { return mProgress.at(mCurrentProgress); }
private:
    void addProgress(QStringList &pids);
private:
    int mCurrentProgress;
    QString mSerialNumber;
    QString mProduct;
    QString mModel;
    QString mDevice;
    QList<Progress> mProgress;
};
typedef std::shared_ptr<device> dev_ptr;

#endif // DEVICE_H
