#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow window(nullptr);
    window.resize(1920, 1080);
    window.show();
    return a.exec();
}
git