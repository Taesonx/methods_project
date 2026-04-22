#include "tcpclient.h"
#include <QDebug>

TcpClient& TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    m_isConnected = false;

    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

TcpClient::~TcpClient()
{
    if (socket->isOpen()) {
        socket->close();
    }
}

void TcpClient::connectToServer(const QString& host, quint16 port)
{
    if (socket->isOpen()) {
        socket->close();
    }
    socket->connectToHost(host, port);
    qDebug() << "Подключение к серверу" << host << ":" << port;
}

void TcpClient::disconnectFromServer()
{
    if (socket->isOpen()) {
        socket->close();
    }
}

bool TcpClient::isConnected() const
{
    return m_isConnected;
}

void TcpClient::sendRequest(const QString& endpoint, const QJsonObject& data)
{
    if (!socket->isOpen() || socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Ошибка: сокет не подключен";
        emit errorOccurred("Нет подключения к серверу");
        return;
    }

    currentEndpoint = endpoint;

    QJsonObject request;
    request["endpoint"] = endpoint;
    request["data"] = data;

    QJsonDocument doc(request);
    QByteArray jsonData = doc.toJson();

    // Формируем HTTP-подобный запрос
    QString httpRequest = QString("POST %1 HTTP/1.1\r\n"
                                  "Content-Type: application/json\r\n"
                                  "Content-Length: %2\r\n"
                                  "\r\n"
                                  "%3")
                              .arg(endpoint)
                              .arg(jsonData.size())
                              .arg(QString::fromUtf8(jsonData));

    socket->write(httpRequest.toUtf8());
    socket->flush();

    qDebug() << "Отправлен запрос на" << endpoint;
}

void TcpClient::onConnected()
{
    m_isConnected = true;
    qDebug() << "Подключено к серверу!";
    emit connected();
}

void TcpClient::onDisconnected()
{
    m_isConnected = false;
    qDebug() << "Отключено от сервера";
    emit disconnected();
}

void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    QString errorMsg = socket->errorString();
    qDebug() << "Ошибка сокета:" << errorMsg;
    emit errorOccurred(errorMsg);
}

void TcpClient::onReadyRead()
{
    buffer.append(socket->readAll());

    QString data = QString::fromUtf8(buffer);

    if (data.contains("\r\n\r\n")) {
        QStringList parts = data.split("\r\n\r\n");
        if (parts.size() >= 2) {
            QString body = parts[1];
            buffer.clear();
            parseResponse(body);
        }
    }
}

void TcpClient::parseResponse(const QString& response)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Ошибка парсинга JSON:" << parseError.errorString();
        emit errorOccurred("Ошибка обработки ответа сервера");
        return;
    }

    QJsonObject jsonResponse = doc.object();
    QString status = jsonResponse["status"].toString();
    QString endpoint = currentEndpoint;

    emit responseReceived(endpoint, jsonResponse);

    if (endpoint == "/login") {
        bool success = (status == "success");
        QString message = jsonResponse["message"].toString();
        emit loginResponse(success, message);
    }
    else if (endpoint == "/register") {
        bool success = (status == "success");
        QString message = jsonResponse["message"].toString();
        emit registerResponse(success, message);
    }
}
