#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class RequestHandler;

class MyTcpServer : public QObject
{
    Q_OBJECT

public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

    bool start(quint16 port);

public slots:
    void slotNewConnection();
    void slotReadyRead();
    void slotClientDisconnected();

private:
    QTcpServer* mTcpServer;
    QTcpSocket* mTcpSocket;
    RequestHandler* m_handler;
    QMap<QTcpSocket*, int> m_clients;
    QMap<QTcpSocket*, QString> m_roles;
};

#endif // MYTCPSERVER_H
