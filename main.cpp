// This app works like Boomerang application with some additional features

#include "mainwindow.h"

#include <QApplication>
#include <QSize>
#include <QIcon>

#include "app_config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(QSize(APP_DISPLAY_WIDTH, APP_DISPLAY_HEIGHT));
    w.setWindowIcon(QIcon(":/icons/boomerang.svg"));
    w.setWindowTitle("Boomerang(ish)");
    w.show();
    return a.exec();
}
