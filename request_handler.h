#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <QString>

class RequestHandler {
public:
    RequestHandler();
    QString handle(const QString& request, int& userId, QString& role);
private:
    QString handleAuth(const QString& login, const QString& password, int& userId, QString& role);
    QString handleReg(const QString& login, const QString& password, const QString& email);
    QString handleReset(const QString& login, const QString& email);
    QString handleStat(int userId);
    QString handleCheck(int userId, double x, double a, double b, double c, double d, double e);
    QString handleAdminStats();
    QString handleAdminReset(const QString& login);
};

#endif
