#include "mytcpserver.h"
#include "request_handler.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);
    m_handler = new RequestHandler();
}

MyTcpServer::~MyTcpServer()
{
    if (mTcpServer->isListening()) {
        mTcpServer->close();
        qDebug() << "Server stopped!";
    }
    delete m_handler;
}

bool MyTcpServer::start(quint16 port)
{
    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Server is not started! Error:" << mTcpServer->errorString();
        return false;
    }

    qDebug() << "========================================";
    qDebug() << "Server is started on port" << port;
    qDebug() << "Waiting for connections...";
    qDebug() << "========================================";
    return true;
}

void MyTcpServer::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    m_clients[mTcpSocket] = -1;
    m_roles[mTcpSocket] = "none";

    mTcpSocket->write("Connected to Calculation Server!\r\n");
    mTcpSocket->flush();

    connect(mTcpSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotReadyRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

    qDebug() << "New client connected!";
    qDebug() << "Total connected clients:" << m_clients.size();
}

void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "Client disconnected!";
    m_clients.remove(mTcpSocket);
    m_roles.remove(mTcpSocket);
    mTcpSocket->deleteLater();
    qDebug() << "Total connected clients left:" << m_clients.size();
}

void MyTcpServer::slotReadyRead()
{
    QByteArray data = mTcpSocket->readAll();
    QString receivedStr = QString::fromUtf8(data).trimmed();
    qDebug() << "Received:" << receivedStr;

    int& userId = m_clients[mTcpSocket];
    QString& role = m_roles[mTcpSocket];

    QString response = m_handler->handle(receivedStr, userId, role);

    mTcpSocket->write(response.toUtf8() + "\n");
    mTcpSocket->flush();

    qDebug() << "Response:" << response.trimmed();
}
