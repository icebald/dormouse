#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "dev/device.h"

class QTextEdit;
class QDockWidget;
class AndroidDevices;
class QTableWidget;
class QStatusBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent);
    bool event(QEvent *event);

protected:
    void setAdbVersion(QString version);

private slots:
    void boxChange(int index);
private:
    void initDocks();
    void setupMenuBar();
    void initDevices();
    void initEdit();
    void refreshDevices(const QList<dev_ptr> &devices);
    void selectDevice(dev_ptr dev);

    QStatusBar *mStatusBar;
    QDockWidget *mDockLeft;
    QDockWidget *mDockRight;
    QDockWidget *mDockBottom;
    QDockWidget *mDockCenter;
    AndroidDevices *mDevices;
    QTextEdit *mEdit;
    QTableWidget *mGrid;
};

#endif // MAINWINDOW_H
