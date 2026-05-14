#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>          // Базовый класс для всех Qt-объектов (сигналы/слоты)
#include <QTcpSocket>       // TCP-сокет — инструмент для связи по сети
#include <QJsonObject>      // JSON-объект для передачи данных
#include <QJsonDocument>    // Документ JSON
#include <QJsonParseError>  // Ошибки парсинга JSON

class TcpClient : public QObject
{
    Q_OBJECT

public:
    static TcpClient& getInstance(); // Синглтон: получить единственный экземпляр класса

    void connectToServer(const QString& host, quint16 port); // Подключиться к серверу (host — IP/имя, port — порт)
    void disconnectFromServer(); // Отключиться от сервера
    bool isConnected() const; // Проверить, подключены ли сейчас

    void sendRequest(const QString& endpoint, const QJsonObject& data); // Отправить запрос на сервер

signals:
    void connected(); // Подключились к серверу
    void disconnected();  // Отключились от сервера
    void errorOccurred(const QString& error); // Произошла ошибка
    void responseReceived(const QString& endpoint, const QJsonObject& response); // Получен ответ
    void loginResponse(bool success, const QString& message); // Ответ на авторизацию
    void registerResponse(bool success, const QString& message); // Ответ на регистрацию
    void passwordResetResponse(bool success, const QString& newPassword); // Ответ на сброс пароля

private slots:
    void onConnected();  // Срабатывает, когда сокет подключился
    void onDisconnected(); // Срабатывает, когда сокет отключился
    void onError(QAbstractSocket::SocketError socketError); // Срабатывает при ошибке
    void onReadyRead(); // Срабатывает, когда пришли данные от сервера

private:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();
    TcpClient(const TcpClient&) = delete; // Запрещаем копирование (синглтон должен быть один)
    TcpClient& operator=(const TcpClient&) = delete;

    void parseResponse(const QString& response);

    QTcpSocket* socket; // Указатель на TCP-сокет (сам инструмент связи)
    QString currentEndpoint; // Какой endpoint сейчас обрабатываем
    bool m_isConnected; // Флаг: подключены или нет (m_ — префикс для member-переменных)
    QByteArray buffer; // Буфер для накопления данных от сервера
};

#endif // TCPCLIENT_H
