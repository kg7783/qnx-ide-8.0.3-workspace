#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("QNX File Manager");

    MainWindow w;
    w.show();

    return app.exec();
}
