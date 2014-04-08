// main.cpp

#include <QApplication>
#include "PAnatomyAnnotator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PAnatomyAnnotator viewer;
    viewer.show();
    return app.exec();
}

