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

    ~DatabaseManager();
};

#endif
