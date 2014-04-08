// main.cpp

#include <QApplication>
#include "PStrokeAnalyser.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PStrokeAnalyser viewer;
    viewer.show();
    return app.exec();
}

