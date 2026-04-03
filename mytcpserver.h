#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>

class MyTcpServer : public QObject
{
    Q_OBJECT

public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

public slots:
    void slotNewConnection();        // когда подключается новый клиент
    void slotClientDisconnected();   // когда клиент отключился
    void slotServerRead();           // когда пришли данные от клиента

private:
    QTcpServer * mTcpServer;
    QTcpSocket * mTcpSocket;
};

#endif // MYTCPSERVER_H
