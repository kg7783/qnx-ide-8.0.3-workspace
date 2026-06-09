#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Qt6 on QNX");
    window.resize(400, 300);

    auto *layout = new QVBoxLayout(&window);

    auto *label = new QLabel("Hello from Qt6 on QNX 8.0!");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    auto *button = new QPushButton("Close");
    QObject::connect(button, &QPushButton::clicked, &app, &QApplication::quit);
    layout->addWidget(button);

    window.show();
    return app.exec();
}
