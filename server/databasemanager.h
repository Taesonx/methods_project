#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QVector>

/*
 * Класс-одиночка (Singleton) для работы с SQLite.
 * Используется для всех операций с БД: регистрация, авторизация,
 * сохранение результатов вычислений, получение статистики.
 */
class DatabaseManager {
private:
    QSqlDatabase db;  // Объект подключения к базе данных SQLite

    // Приватный конструктор — запрещаем прямое создание объектов (Singleton)
    DatabaseManager();
    // Запрещаем копирование и присваивание
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Хэширование пароля через SHA-256
    QString hashPassword(const QString& password);

public:
    // Статический метод доступа к единственному экземпляру
    static DatabaseManager& getInstance();
    ~DatabaseManager();

    // Управление соединением с БД
    bool connect();
    void close();
    bool isOpen() const;

    // Операции с пользователями
    bool registerUser(const QString& name, const QString& login, const QString& password);
    int loginUser(const QString& login, const QString& password);
    QString getUserRole(int userId);

    // Сброс пароля (только админский)
    QString adminResetPassword(const QString& login);

    // Сохранение результата вычисления
    bool save(int userId, double x, double a, double b, double c, double d, double e, double result);

    // Статистика пользователя
    QVector<int> getStats(int userId);
    void updateStats(int userId, bool solved, int timeSpent = 1);

    // Админская статистика
    int getUserCount();
    int getTodayRegisteredCount();
    int getTotalCalculationsCount();
    int getUserCalculationsCount(int userId);
    int getActiveUserCount();
    double getAvgCalculationsPerUser();
};

#endif // DATABASEMANAGER_H
