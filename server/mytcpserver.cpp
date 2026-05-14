#include "mytcpserver.h"
#include "request_handler.h"
#include <QDebug>

/*
 * КОНСТРУКТОР: MyTcpServer
 * Создаёт:
 *   - QTcpServer (для приёма соединений)
 *   - RequestHandler (для обработки команд)
 *
 * Сервер ещё не запущен — только созданы объекты.
 */
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);   // this = родитель, будет удалён автоматически
    m_handler = new RequestHandler();     // создаём обработчик запросов
}

/*
 * ДЕСТРУКТОР: ~MyTcpServer
 * Если сервер ещё слушает порт — закрываем его.
 * Удаляем обработчик запросов.
 */
MyTcpServer::~MyTcpServer()
{
    if (mTcpServer->isListening()) {
        mTcpServer->close();
        qDebug() << "Server stopped!";
    }
    delete m_handler;   // явно удаляем, так как создавали через new
}

/*
 * start()
 * Запускает сервер на указанном порту.
 *
 * Алгоритм:
 *   1. Подключаем сигнал newConnection к слоту slotNewConnection
 *   2. Вызываем listen() на всех интерфейсах (Any) и заданном порту
 *   3. При ошибке выводим сообщение и возвращаем false
 *   4. При успехе выводим информацию и возвращаем true
 */
bool MyTcpServer::start(quint16 port)
{
    // Сигнал newConnection генерируется, когда клиент подключается
    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    // Начинаем слушать. QHostAddress::Any = все сетевые интерфейсы (127.0.0.1, LAN IP, и т.д.)
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

/*
 * slotNewConnection()
 * Вызывается автоматически при подключении нового клиента.
 * Подробности:
 *   1. nextPendingConnection() — принимает соединение, возвращает QTcpSocket*
 *   2. Сохраняем начальное состояние: user_id = -1, role = "none"
 *   3. Отправляем приветствие (можно убрать, клиент может его не ждать)
 *   4. Подключаем сигналы сокета:
 *      - readyRead → когда клиент что-то прислал
 *      - disconnected → когда клиент отключился
 *
 * ВНИМАНИЕ: Переменная mTcpSocket будет указывать на ПОСЛЕДНЕГО подключившегося
 * клиента. Для многоклиентности мы храним все сокеты в m_clients и m_roles.
 * Сигналы сокета будут вызывать слоты, и в них mTcpSocket будет указывать
 * на тот сокет, который сгенерировал сигнал (благодаря контексту).
 */
void MyTcpServer::slotNewConnection()
{
    // Принимаем соединение
    mTcpSocket = mTcpServer->nextPendingConnection();

    // Инициализируем состояние клиента (не авторизован)
    m_clients[mTcpSocket] = -1;          // user_id = -1
    m_roles[mTcpSocket] = "none";        // роль не определена

    // Приветственное сообщение (необязательно)
    mTcpSocket->write("Connected to Calculation Server!\r\n");
    mTcpSocket->flush();                 // Немедленно отправляем

    // Подключаем сигналы сокета
    connect(mTcpSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotReadyRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

    // Логирование
    qDebug() << "New client connected!";
    qDebug() << "Total connected clients:" << m_clients.size();
}

/*
 * slotReadyRead()

 * Вызывается, когда от клиента пришли данные.
 *
 * Алгоритм:
 *   1. Читаем ВСЕ данные через readAll() (может быть несколько сообщений,
 *      но для простоты считаем, что клиент шлит по одной команде)
 *   2. Преобразуем QByteArray в QString (UTF-8)
 *   3. Удаляем лишние пробелы и переводы строк (.trimmed())
 *   4. Получаем состояние текущего клиента (ссылки для возможности изменения)
 *   5. Передаём запрос в RequestHandler::handle()
 *   6. Отправляем ответ, добавляя '\n' для удобства клиента
 */
void MyTcpServer::slotReadyRead()
{
    // Читаем все данные из сокета
    QByteArray data = mTcpSocket->readAll();
    QString receivedStr = QString::fromUtf8(data).trimmed();
    qDebug() << "Received:" << receivedStr;

    // Получаем состояние клиента (ссылку, чтобы можно было изменить)
    int& userId = m_clients[mTcpSocket];
    QString& role = m_roles[mTcpSocket];

    // Обрабатываем запрос
    QString response = m_handler->handle(receivedStr, userId, role);

    // Отправляем ответ
    mTcpSocket->write(response.toUtf8() + "\n");
    mTcpSocket->flush();

    qDebug() << "Response:" << response.trimmed();
}

/*
 * slotClientDisconnected()
 * Вызывается при отключении клиента.
 * Действия:
 *   1. Удаляем информацию о клиенте из словарей
 *   2. Удаляем сокет через deleteLater() (безопасное удаление в Qt)
 *   3. Выводим количество оставшихся клиентов
 */
void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "Client disconnected!";

    // Удаляем записи о клиенте
    m_clients.remove(mTcpSocket);
    m_roles.remove(mTcpSocket);

    // Безопасное удаление сокета (Qt сам его удалит после завершения текущих операций)
    mTcpSocket->deleteLater();

    qDebug() << "Total connected clients left:" << m_clients.size();
}
