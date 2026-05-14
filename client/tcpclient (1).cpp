#include "tcpclient.h"
#include <QDebug>

TcpClient& TcpClient::getInstance()
{
    static TcpClient instance; // Создаётся один раз при первом вызове
    return instance;           // Все последующие вызовы возвращают тот же объект
}

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this); // Создаём TCP-сокет (инструмент для связи по сети)
    m_isConnected = false;         // Изначально не подключены
    buffer.clear();                // Очищаем буфер (будет накапливать данные от сервера)

    // Подключаем сигналы сокета
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

TcpClient::~TcpClient()
{
    // Если уже были подключены — закрываем старое соединение
    if (socket->isOpen()) {
        socket->close(); // Закрываем соединение, если оно было открыто
    }
}

void TcpClient::connectToServer(const QString& host, quint16 port)
{
    if (socket->isOpen()) {
        socket->close();
    }
    // Подключкемся к серверу
    socket->connectToHost(host, port);
    qDebug() << "Подключение к серверу" << host << ":" << port;
}

// Отключение от сервера
void TcpClient::disconnectFromServer()
{
    if (socket->isOpen()) {
        socket->close();
    }
}

// Проверка подключены ли мы к серверу
bool TcpClient::isConnected() const
{
    return m_isConnected;
}

// Отправка запроса на сервер
void TcpClient::sendRequest(const QString& endpoint, const QJsonObject& data)
{
    if (!socket->isOpen() || socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Ошибка: сокет не подключен";
        emit errorOccurred("Нет подключения к серверу");
        return;
    }

    currentEndpoint = endpoint;

    QString request;

    // Формируем запрос в формате, который понимает сервер
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

    // Отправляем запрос через сокет
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
    emit errorOccurred(errorMsg); // Оповещаем контроллер об ошибке
}

void TcpClient::onReadyRead()
{
    // 1. Читаем ВСЁ, что пришло от сервера, и добавляем в буфер
    buffer.append(socket->readAll());

    // 2. Переводим байты в строку
    QString data = QString::fromUtf8(buffer);

    // 3. Проверяем, есть ли полное сообщение (заканчивается на \n)
    if (data.contains('\n')) {
        // 4. Разбиваем буфер на строки (вдруг пришло сразу несколько сообщений)
        QStringList lines = data.split('\n');
        // 5. Обрабатываем каждую строку
        for (const QString& line : lines) {
            if (!line.trimmed().isEmpty()) {
                parseResponse(line.trimmed());
            }
        }
        // 6. Очищаем буфер для следующих сообщений
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
