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
    buffer.clear();

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

    QString request;

    // Формируем запрос в формате, который понимает сервер Даши
    if (endpoint == "/login") {
        QString login = data["login"].toString();
        QString password = data["password"].toString();
        request = QString("auth&%1&%2").arg(login).arg(password);
    }
    else if (endpoint == "/register") {
        QString login = data["login"].toString();
        QString password = data["password"].toString();
        QString email = data["email"].toString();
        request = QString("reg&%1&%2&%3").arg(login).arg(password).arg(email);
    }
    else if (endpoint == "/reset") {
        QString email = data["email"].toString();
        request = QString("reset&%1").arg(email);
    }
    else if (endpoint == "/param_change") {
        double a = data["a"].toDouble();
        double b = data["b"].toDouble();
        double c = data["c"].toDouble();
        double d = data["d"].toDouble();
        double e = data["e"].toDouble();
        request = QString("params&%1&%2&%3&%4&%5")
                      .arg(a).arg(b).arg(c).arg(d).arg(e);
    }
    else if (endpoint == "/calculate") {
        double x = data["x"].toDouble();
        double a = data["a"].toDouble();
        double b = data["b"].toDouble();
        double c = data["c"].toDouble();
        double d = data["d"].toDouble();
        double e = data["e"].toDouble();
        request = QString("check&0&29&%1&%2&%3&%4&%5&%6")
                      .arg(x).arg(a).arg(b).arg(c).arg(d).arg(e);
    }
    else {
        qDebug() << "Неизвестный endpoint:" << endpoint;
        return;
    }

    socket->write(request.toUtf8() + "\n");
    socket->flush();

    qDebug() << "Отправлен запрос:" << request;
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
    Q_UNUSED(socketError);
    QString errorMsg = socket->errorString();
    qDebug() << "Ошибка сокета:" << errorMsg;
    emit errorOccurred(errorMsg);
}

void TcpClient::onReadyRead()
{
    buffer.append(socket->readAll());

    QString data = QString::fromUtf8(buffer);

    // Проверяем, есть ли полный ответ (заканчивается на \n)
    if (data.contains('\n')) {
        QStringList lines = data.split('\n');
        for (const QString& line : lines) {
            if (!line.trimmed().isEmpty()) {
                parseResponse(line.trimmed());
            }
        }
        buffer.clear();
    }
}

void TcpClient::parseResponse(const QString& response)
{
    qDebug() << "Получен ответ:" << response;

    // Парсим ответ сервера в формате Даши
    if (response.startsWith("auth+")) {
        // auth+&login&role
        QStringList parts = response.split('&');
        bool success = true;
        QString message = "Успешный вход";
        if (parts.size() >= 2) {
            message = QString("Добро пожаловать, %1!").arg(parts[1]);
        }
        emit loginResponse(success, message);
    }
    else if (response.startsWith("auth-")) {
        emit loginResponse(false, "Неверный логин или пароль");
    }
    else if (response.startsWith("reg+")) {
        // reg+&login
        emit registerResponse(true, "Регистрация успешна");
    }
    else if (response.startsWith("reg-")) {
        emit registerResponse(false, "Пользователь уже существует");
    }
    else if (response.startsWith("reset+")) {
        // reset+&newpassword
        QStringList parts = response.split('&');
        if (parts.size() >= 2) {
            QString newPass = parts[1];
            emit passwordResetResponse(true, newPass);
        } else {
            emit passwordResetResponse(true, "Пароль сброшен");
        }
    }
    else if (response.startsWith("reset-")) {
        emit passwordResetResponse(false, "Пользователь с таким email не найден");
    }
    else if (response.startsWith("params+")) {
        // params+&ok - параметры успешно сохранены
        qDebug() << "Параметры успешно сохранены на сервере";
    }
    else if (response.startsWith("params-")) {
        // params-&ошибка параметров
        qDebug() << "Ошибка сохранения параметров на сервере:" << response;
    }
    else if (response.startsWith("check+")) {
        // check+ - вычисление успешно сохранено
        qDebug() << "Вычисление успешно сохранено на сервере";
    }
    else if (response.startsWith("check-")) {
        qDebug() << "Ошибка вычисления на сервере";
    }
    else if (response == "Connected to Calculation Server!") {
        qDebug() << "Приветствие от сервера";
    }
    else {
        qDebug() << "Неизвестный формат ответа:" << response;
    }
}
