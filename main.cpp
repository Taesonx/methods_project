#include <QCoreApplication>
#include "mytcpserver.h"
#include "databasemanager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "  EchoServer - Variant 29";
    qDebug() << "  Multi-client TCP Server";

    if (!DatabaseManager::getInstance().connect()) {
        qDebug() << "[FATAL] Database connection failed!";
        return 1;
    }
    qDebug() << "[OK] Database connected successfully!";

    MyTcpServer server;
    if (!server.start(8080)) {
        return 1;
    }
    qDebug() << "  Server is running. Waiting...";
    return a.exec();
}
