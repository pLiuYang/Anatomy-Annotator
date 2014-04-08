// main.cpp

#include <QApplication>
#include "PConsole.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PConsole console;
    console.show();
    return app.exec();
}

