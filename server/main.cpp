#include <QCoreApplication>
#include "mytcpserver.h"
#include "databasemanager.h"
#include <QDebug>

/*
 * ТОЧКА ВХОДА (main)
 * 1. Создаём QCoreApplication (консольное приложение, без GUI)
 * 2. Подключаемся к базе данных (DatabaseManager::connect())
 * 3. Создаём и запускаем TCP-сервер на порту 8080
 * 4. Запускаем цикл обработки событий QCoreApplication::exec()
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "  EchoServer - Variant 29";
    qDebug() << "  Multi-client TCP Server";

    // ========== ИНИЦИАЛИЗАЦИЯ БАЗЫ ДАННЫХ ==========
    if (!DatabaseManager::getInstance().connect()) {
        qDebug() << "[FATAL] Database connection failed!";
        return 1;
    }
    qDebug() << "[OK] Database connected successfully!";

    // ========== ЗАПУСК СЕРВЕРА ==========
    MyTcpServer server;
    if (!server.start(8080)) {
        return 1;
    }

    qDebug() << "  Server is running. Waiting...";

    // ========== ЗАПУСК ГЛАВНОГО ЦИКЛА ==========
    return a.exec();
}
