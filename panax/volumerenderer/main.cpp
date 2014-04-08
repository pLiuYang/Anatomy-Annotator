// main.cpp

#include <QApplication>
#include "PVolumeRenderer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PVolumeRenderer viewer;
    viewer.show();
    return app.exec();
}

