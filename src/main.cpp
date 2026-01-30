#include <QApplication>
#include <QScreen>
#include <iostream>
#include <QDebug>
#include "mediacontroller.h"
#include "videoplayer.h"


int main(int argc, char *argv[])
{

    std::cout << "Launching" << std::endl;

    QApplication app(argc, argv);

    PlayerWindow window;

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    QSize startSize = screenGeometry.size() * 0.8;
    window.resize(startSize);

    window.move(
        (screenGeometry.width() - startSize.width()) / 2,
        (screenGeometry.height() - startSize.height()) / 2
    );

    window.show();

    return app.exec();
}
