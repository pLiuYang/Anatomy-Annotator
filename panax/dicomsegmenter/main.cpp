// main.cpp

#include <QApplication>
#include "PDicomSegmenter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PDicomSegmenter viewer;
    viewer.show();
    return app.exec();
}

