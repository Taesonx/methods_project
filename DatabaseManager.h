#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>

class DatabaseManager {
private:
    QSqlDatabase db;

    DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QString hashPassword(const QString& password);

public:
    static DatabaseManager& getInstance();

    bool connect();
    void close();

    bool registerUser(const QString& name, const QString& login, const QString& password, const QString& mail);
    int loginUser(const QString& login, const QString& password);

    bool save(int userId, double x, double a, double b, double c, double d, double e, double result);
    
    bool resetPassword(const QString& login, const QString& newPassword);

    // Статистика
    int getUserCount();              // кол-во всех пользователей
    int getTodayRegisteredCount();   // кол-во зарегистрированных сегодня
    int getTotalCalculationsCount(); // кол-во всех вычислений
    int getUserCalculationsCount(int userId); // вычислений пользователя
    int getActiveUserCount();        // кто хотя бы раз вычислял
};

    ~DatabaseManager();

   

#endif
