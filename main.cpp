#include <QCoreApplication>
#include "mytcpserver.h"
#include "DatabaseManager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (!DatabaseManager::getInstance().connect()) {
        qDebug() << "Database connection failed!";
    }

    MyTcpServer server;

    return a.exec();
}
