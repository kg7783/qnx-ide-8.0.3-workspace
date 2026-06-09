#include <QApplication>
#include "plasmawidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PlasmaWidget w;
    w.setWindowTitle("Qt6 Plasma");
    w.show();

    return app.exec();
}
