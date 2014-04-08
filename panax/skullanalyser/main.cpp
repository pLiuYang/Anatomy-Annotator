// main.cpp

#include <QApplication>
#include "PSkullAnalyser.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PSkullAnalyser viewer;
    viewer.show();
    return app.exec();
}

