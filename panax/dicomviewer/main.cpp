// main.cpp

#include <QApplication>
#include "PDicomViewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PDicomViewer viewer;
    viewer.show();
    return app.exec();
}

