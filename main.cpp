#include <QApplication>
#include "functionsforclient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FunctionsForClient controller;

    return app.exec();
}
