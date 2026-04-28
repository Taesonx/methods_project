#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

class TcpClient : public QObject
{
    Q_OBJECT

public:
    static TcpClient& getInstance();

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;

    void sendRequest(const QString& endpoint, const QJsonObject& data);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);
    void responseReceived(const QString& endpoint, const QJsonObject& response);
    void loginResponse(bool success, const QString& message);
    void registerResponse(bool success, const QString& message);
    void passwordResetResponse(bool success, const QString& newPassword);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    void parseResponse(const QString& response);

    QTcpSocket* socket;
    QString currentEndpoint;
    bool m_isConnected;
    QByteArray buffer;
};

#endif // TCPCLIENT_H
