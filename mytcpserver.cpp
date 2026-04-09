#include "mytcpserver.h"
#include "function29.h"
#include "DatabaseManager.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 8080)){
        qDebug() << "Server is not started! Error:" << mTcpServer->errorString();
    } else {
        qDebug() << "Server is started on port 8080";
    }
}

MyTcpServer::~MyTcpServer()
{
    if(mTcpServer) {
        mTcpServer->close();
    }
}

void MyTcpServer::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    mTcpSocket->write("Connected to Calculation Server!\r\n");

    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &MyTcpServer::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected,
            this, &MyTcpServer::slotClientDisconnected);

    qDebug() << "New client connected!";
}

void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "Client disconnected!";
    mTcpSocket->close();
}

void MyTcpServer::slotServerRead()
{
    QByteArray data = mTcpSocket->readAll();
    QString receivedStr = QString::fromUtf8(data).trimmed();
    qDebug() << "Received:" << receivedStr;

    QStringList parts = receivedStr.split(',');

    if (parts.size() != 6) {
        mTcpSocket->write("ERROR: Need 6 numbers: x,a,b,c,d,e\n");
        return;
    }

    bool ok1, ok2, ok3, ok4, ok5, ok6;
    double x = parts[0].toDouble(&ok1);
    double a = parts[1].toDouble(&ok2);
    double b = parts[2].toDouble(&ok3);
    double c = parts[3].toDouble(&ok4);
    double d = parts[4].toDouble(&ok5);
    double e = parts[5].toDouble(&ok6);

    if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6) {
        mTcpSocket->write("ERROR: Invalid numbers\n");
        return;
    }

    double result = f29(x, a, b, c, d, e);

    int userId = 1;
    DatabaseManager::getInstance().save(userId, x, a, b, c, d, e, result);

    QString response = QString::number(result) + "\n";
    mTcpSocket->write(response.toUtf8());

    qDebug() << "f(" << x << "," << a << "," << b << "," << c << "," << d << "," << e << ") = " << result;
}
