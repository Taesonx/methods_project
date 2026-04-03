#include "mytcpserver.h"
#include <QDebug>

// Конструктор: запускает сервер
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    // Подключаем сигнал newConnection к слоту
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    // Запускаем сервер на порту 8080
    if(!mTcpServer->listen(QHostAddress::Any, 8080)){
        qDebug() << "Server is not started! Error:" << mTcpServer->errorString();
    } else {
        qDebug() << "Server is started on port 8080";
    }
}

// Деструктор: закрываем сервер
MyTcpServer::~MyTcpServer()
{
    if(mTcpServer) {
        mTcpServer->close();
    }
}

// Слот: новый клиент подключился
void MyTcpServer::slotNewConnection()
{
    // Получаем сокет нового клиента
    mTcpSocket = mTcpServer->nextPendingConnection();

    // Отправляем приветствие
    mTcpSocket->write("Connected to Echo Server!\r\n");

    // Подключаем сигналы сокета
    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &MyTcpServer::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected,
            this, &MyTcpServer::slotClientDisconnected);

    qDebug() << "New client connected!";
}

// Слот: клиент отключился
void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "Client disconnected!";
    mTcpSocket->close();
}

// Слот: пришли данные от клиента
void MyTcpServer::slotServerRead()
{
    // Читаем все данные
    QByteArray data = mTcpSocket->readAll();
    qDebug() << "Received:" << data;

    // Отправляем обратно (эхо)
    mTcpSocket->write(data);
}
