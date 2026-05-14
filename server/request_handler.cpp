#include "request_handler.h"
#include "databasemanager.h"
#include "function29.h"
#include <QDebug>
#include <QStringList>

RequestHandler::RequestHandler() {}

QString RequestHandler::handle(const QString& request, int& userId, QString& role)
{
    QStringList parts = request.split('&');
    if (parts.isEmpty()) return "ERROR: empty request";

    QString cmd = parts[0];
    qDebug() << "Command:" << cmd;

    // команды без аутентификации
    if (cmd == "reg") {
        if (parts.size() < 4) return "reg-";
        return handleReg(parts[1], parts[2], parts[3]);
    }
    if (cmd == "auth") {
        if (parts.size() < 3) return "auth-";
        return handleAuth(parts[1], parts[2], userId, role);
    }
    if (cmd == "reset") {
        // reset&email  (без логина)
        if (parts.size() < 2) return "reset-";
        return handleResetByEmail(parts[1]);
    }
    if (cmd == "params") {
        if (parts.size() >= 6) {
            double a = parts[1].toDouble();
            double b = parts[2].toDouble();
            double c = parts[3].toDouble();
            double d = parts[4].toDouble();
            double e = parts[5].toDouble();
            qDebug() << "params updated:" << a << b << c << d << e;
            return "params+";
        }
        return "params-";
    }

    // команды, требующие аутентификации
    if (userId == -1) return "ERROR: not authenticated";

    if (cmd == "stat") return handleStat(userId);
    if (cmd == "check") {
        if (parts.size() < 9) return "check-";
        double x = parts[3].toDouble();
        double a = parts[4].toDouble();
        double b = parts[5].toDouble();
        double c = parts[6].toDouble();
        double d = parts[7].toDouble();
        double e = parts[8].toDouble();
        return handleCheck(userId, x, a, b, c, d, e);
    }

    // админские команды
    if (cmd == "admin_stats") {
        if (role != "admin") return "ERROR: access denied";
        return handleAdminStats();
    }
    if (cmd == "admin_reset") {
        if (role != "admin") return "ERROR: access denied";
        if (parts.size() < 2) return "ERROR: need login";
        return handleAdminReset(parts[1]);
    }

    return "ERROR: unknown command";
}

QString RequestHandler::handleAuth(const QString& login, const QString& password, int& userId, QString& role)
{
    int id = DatabaseManager::getInstance().loginUser(login, password);
    if (id != -1) {
        userId = id;
        role = DatabaseManager::getInstance().getUserRole(id);
        return "auth+&" + login + "&" + role;
    }
    return "auth-";
}

QString RequestHandler::handleReg(const QString& login, const QString& password, const QString& email)
{
    bool ok = DatabaseManager::getInstance().registerUser(login, login, password, email);
    return ok ? "reg+&" + login : "reg-";
}

// старый метод (не используется)
QString RequestHandler::handleReset(const QString& login, const QString& email)
{
    QString newPass = DatabaseManager::getInstance().resetPassword(login, email);
    return newPass.isEmpty() ? "reset-" : "reset+&" + newPass;
}

// НОВЫЙ МЕТОД – сброс по одному email
QString RequestHandler::handleResetByEmail(const QString& email)
{
    QString newPass = DatabaseManager::getInstance().resetPasswordByEmail(email);
    return newPass.isEmpty() ? "reset-" : "reset+&" + newPass;
}

QString RequestHandler::handleStat(int userId)
{
    QVector<int> stats = DatabaseManager::getInstance().getStats(userId);
    return QString("stat&%1$%2&%3").arg(stats[0]).arg(stats[1]).arg(stats[2]);
}

QString RequestHandler::handleCheck(int userId, double x, double a, double b, double c, double d, double e)
{
    double result = calculateFunction(x, a, b, c, d, e);
    DatabaseManager::getInstance().save(userId, x, a, b, c, d, e, result);
    DatabaseManager::getInstance().updateStats(userId, true, 1);
    return "check+";
}

QString RequestHandler::handleAdminStats()
{
    int userCount = DatabaseManager::getInstance().getUserCount();
    int totalCalcs = DatabaseManager::getInstance().getTotalCalculationsCount();
    int activeUsers = DatabaseManager::getInstance().getActiveUserCount();
    double avgCalcs = DatabaseManager::getInstance().getAvgCalculationsPerUser();
    return QString("admin_stats&%1&%2&%3&%4")
        .arg(userCount)
        .arg(totalCalcs)
        .arg(activeUsers)
        .arg(avgCalcs, 0, 'f', 2);
}

QString RequestHandler::handleAdminReset(const QString& login)
{
    QString newPass = DatabaseManager::getInstance().adminResetPassword(login);
    return newPass.isEmpty() ? "admin_reset-" : "admin_reset+&" + newPass;
}
