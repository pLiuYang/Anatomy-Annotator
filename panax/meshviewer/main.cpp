// main.cpp

#include <QApplication>
#include "PMeshViewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PMeshViewer viewer;
    viewer.show();
    return app.exec();
}

