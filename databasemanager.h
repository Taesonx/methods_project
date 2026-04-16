#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QVector>

class DatabaseManager {
private:
    QSqlDatabase db;

    DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QString hashPassword(const QString& password);

public:
    static DatabaseManager& getInstance();
    ~DatabaseManager();

    bool connect();
    void close();
    bool isOpen() const;
    bool registerUser(const QString& name, const QString& login, const QString& password, const QString& mail);
    int loginUser(const QString& login, const QString& password);
    QString getUserRole(int userId);
    QString resetPassword(const QString& login, const QString& email);
    QString adminResetPassword(const QString& login);
    bool save(int userId, double x, double a, double b, double c, double d, double e, double result);

    QVector<int> getStats(int userId);
    void updateStats(int userId, bool solved, int timeSpent = 1);

    int getUserCount();
    int getTodayRegisteredCount();
    int getTotalCalculationsCount();
    int getUserCalculationsCount(int userId);
    int getActiveUserCount();
    double getAvgCalculationsPerUser();
};

#endif // DATABASEMANAGER_H
