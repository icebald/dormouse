#include "mainwindow.h"
#include "dev/androiddevices.h"
#include "dev/device.h"
#include "events.h"

#include <QTableWidget>
#include <QDockWidget>
#include <QMenuBar>
#include <QTextEdit>
#include <QDebug>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("hamapi icebear's tool");
    mGrid = nullptr;
    mStatusBar = new QStatusBar(this);
    setStatusBar(mStatusBar);
    setupMenuBar();
    initDocks();
    initDevices();
    initEdit();
}

void MainWindow::initDocks() {
    mDockLeft = new QDockWidget("选中的设备",this);//构建停靠窗口，指定父类
    mDockLeft->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);//设置停靠窗口特性，可移动，可关闭
    mDockLeft->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);//设置可停靠区域为主窗口左边和右边
    addDockWidget(Qt::LeftDockWidgetArea, mDockLeft);

    mDockRight = new QDockWidget("right",this);
    mDockRight->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    mDockRight->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, mDockRight);

    mDockBottom = new QDockWidget("设备",this);
    mDockBottom->setFeatures(QDockWidget::NoDockWidgetFeatures);
    mDockBottom->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, mDockBottom);

    mDockCenter = new QDockWidget("center", this);
    mDockCenter->setFeatures(QDockWidget::NoDockWidgetFeatures);
    setCentralWidget(mDockCenter);
}

void MainWindow::setupMenuBar() {
    QMenu *menu = menuBar()->addMenu(tr("&File"));
    QMenu *toolBarMenu = menuBar()->addMenu(tr("Tools"));
    QMenu *aboutMenu = menuBar()->addMenu(tr("About"));
}

void MainWindow::initDevices() {
    mDevices = new AndroidDevices(this);
    mDevices->start();
}

void MainWindow::initEdit() {
    mEdit = new QTextEdit(mDockCenter);
    mDockCenter->setWidget(mEdit);
}

void MainWindow::boxChange(int index) {
    mDevices->setPidSelect(mGrid->currentRow(), index);
}

void MainWindow::refreshDevices(const QList<dev_ptr> &devices) {
    if (nullptr == mGrid) {
        mGrid = new QTableWidget(mDockBottom);
        mGrid->setColumnCount(3);
        mGrid->setColumnWidth(0, 400);
        mGrid->setColumnWidth(1, 400);
        mDockBottom->setWidget(mGrid);
        mGrid->horizontalHeader()->setStretchLastSection(true);
    }

    mGrid->clear();
    mGrid->setRowCount(devices.size());
    for (int i = 0; i < devices.size(); ++i) {
        qDebug()<<devices.at(i)->getSerialNumber();
        mGrid->setItem(i, 0, new QTableWidgetItem(devices.at(i)->getSerialNumber()));
        mGrid->setItem(i, 1, new QTableWidgetItem(devices.at(i)->getDevice() + " " + devices.at(i)->getProduct()));
        QComboBox *combox = new QComboBox(this);
        QList<Progress> pro = devices.at(i)->getProgress();
        for (int j = 0; j < pro.size(); ++j) {
            combox->addItem(pro.at(j).name + "  " + pro.at(j).pid);
        }
        connect(combox, SIGNAL(currentIndexChanged(int)), this, SLOT(boxChange(int)));
        mGrid->setCellWidget(i, 2, combox);
    }
}

void MainWindow::setAdbVersion(QString version) {
    statusBar()->setStyleSheet("color:green");
    statusBar()->showMessage(version);
}

void MainWindow::selectDevice(dev_ptr dev) {
    dev->catLogcat();
}

bool MainWindow::event(QEvent *ev) {
    if (ev->type() == REFRESH_DEVICE) {
        refreshDevices(((DeviceEvent*) ev)->getDev());
        return true;
    } else if (ev->type() == ADB_VERSION) {
        setAdbVersion(((VersionEvent *) ev)->getVersion());
        return true;
    } else if (ev->type() == SET_SELECT_DEV) {
        selectDevice(((SetSelectDevEvent *)ev)->getDev());
        return true;
    }
    return QMainWindow::event(ev);
}
