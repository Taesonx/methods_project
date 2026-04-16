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

    // ---------- команды без аутентификации ----------
    if (cmd == "reg") {
        if (parts.size() < 4) return "reg-";
        return handleReg(parts[1], parts[2], parts[3]);
    }
    if (cmd == "auth") {
        if (parts.size() < 3) return "auth-";
        return handleAuth(parts[1], parts[2], userId, role);
    }
    if (cmd == "reset") {
        if (parts.size() < 3) return "reset-";
        return handleReset(parts[1], parts[2]);
    }

    // ---------- команды, требующие аутентификации ----------
    if (userId == -1) return "ERROR: not authenticated";

    if (cmd == "stat") {
        return handleStat(userId);
    }
    if (cmd == "check") {
        if (parts.size() < 4) return "check-";
        // taskNum и variant не используются, но читаем, чтобы сдвинуть индексы
        /*int taskNum =*/ parts[1].toInt();
        /*int variant =*/ parts[2].toInt();
        double x = parts[3].toDouble();

        double a = 1.0, b = 1.0, c = 1.0, d = 29.0, e = 29.0;
        if (parts.size() >= 9) {
            a = parts[4].toDouble();
            b = parts[5].toDouble();
            c = parts[6].toDouble();
            d = parts[7].toDouble();
            e = parts[8].toDouble();
        }
        return handleCheck(userId, x, a, b, c, d, e);
    }

    // ---------- админские команды ----------
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

// ========== реализация ==========

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

QString RequestHandler::handleReset(const QString& login, const QString& email)
{
    QString newPass = DatabaseManager::getInstance().resetPassword(login, email);
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
