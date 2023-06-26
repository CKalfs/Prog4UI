#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // For HighDPI screens
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
