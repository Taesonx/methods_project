#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class RequestHandler;

/*
 * КЛАСС: MyTcpServer
 * Назначение: Главный TCP-сервер, принимающий подключения от клиентов.
 * Архитектура:
 *   - Наследуется от QObject для работы с сигналами/слотами
 *   - Использует QTcpServer для прослушивания порта
 *   - Для каждого клиента создаётся QTcpSocket
 *   - Состояние клиентов (user_id и роль) хранится в QMap
 * Потоковая модель:
 *   - Все слоты вызываются в основном потоке (GUI/main thread)
 *   - Асинхронная обработка через сигналы readyRead/disconnected
 * Протокол:
 *   - Текстовый, команды разделяются символом '&'
 *   - Ответы заканчиваются символом новой строки '\n'
 */
class MyTcpServer : public QObject
{
    Q_OBJECT   // Макрос для поддержки сигналов/слотов и MOC (Meta-Object Compiler)

public:
    /*
     * Конструктор
     * @param parent Родительский QObject (для автоматического удаления)
     *
     * Инициализирует:
     *   - mTcpServer: объект, слушающий порт
     *   - m_handler: обработчик бизнес-логики
     */
    explicit MyTcpServer(QObject *parent = nullptr);


    ~MyTcpServer();

    /*
     * Запуск сервера
     * @param port Номер порта для прослушивания (например, 8080)
     * @return true если удалось запустить, false при ошибке
     *
     * Подключает сигнал newConnection к слоту slotNewConnection
     * и начинает слушать все сетевые интерфейсы (QHostAddress::Any)
     */
    bool start(quint16 port);

public slots:
    /*
     * Слот: вызывается при подключении нового клиента
     *
     * Действия:
     *   1. Принимает соединение через nextPendingConnection()
     *   2. Сохраняет сокет в словари m_clients и m_roles (user_id = -1, role = "none")
     *   3. Отправляет приветственное сообщение
     *   4. Подключает сигналы сокета (readyRead, disconnected)
     *
     * ВНИМАНИЕ: mTcpSocket перезаписывается при каждом новом клиенте,
     * но состояние хранится в QMap, поэтому многоклиентность работает.
     */
    void slotNewConnection();

    /*
     * Слот: вызывается при получении данных от клиента
     *
     * Действия:
     *   1. Читает все данные через readAll()
     *   2. Передаёт строку запроса в RequestHandler::handle()
     *   3. Полученный ответ отправляет обратно клиенту
     *
     * Формат: ответ + "\n" (для удобства парсинга на клиенте)
     */
    void slotReadyRead();

    /*
     * Слот: вызывается при отключении клиента
     *
     * Действия:
     *   1. Удаляет данные клиента из словарей m_clients и m_roles
     *   2. Удаляет сокет через deleteLater()
     */
    void slotClientDisconnected();

private:
    // ========== ОБЪЕКТЫ ==========

    QTcpServer* mTcpServer;      // Слушает порт, принимает новые соединения
    QTcpSocket* mTcpSocket;      // Сокет текущего клиента (для сигналов)
    RequestHandler* m_handler;   // Обработчик бизнес-логики (регистрация, авторизация, вычисления)

    // ========== ХРАНЕНИЕ СОСТОЯНИЯ КЛИЕНТОВ ==========
    // Ключ: указатель на сокет (уникальный для каждого клиента)
    // Значение: user_id (-1 если не авторизован)
    QMap<QTcpSocket*, int> m_clients;

    // Ключ: указатель на сокет
    // Значение: роль пользователя ("admin", "user", "none")
    QMap<QTcpSocket*, QString> m_roles;
};

#endif // MYTCPSERVER_H
