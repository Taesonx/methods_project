#ifndef FUNCTIONSFORCLIENT_H
#define FUNCTIONSFORCLIENT_H

#include <QObject>
#include <QMap>
#include "formauth.h"
#include "formreg.h"
#include "formtaskchoice.h"
#include "formsolve.h"
#include "mainwindow.h"
#include "formresetpassword.h"
#include "tcpclient.h"

class FunctionsForClient : public QObject
{
    Q_OBJECT

public:
    explicit FunctionsForClient(QObject *parent = nullptr);
    ~FunctionsForClient();

private slots:
    void processAuth(const QString& login, const QString& password);
    void processRegister(const QString& login, const QString& password, const QString& email);
    void processTaskSelected(int taskId);
    void showMainWindow(double a, double b, double c, double d, double e);
    void processResetPassword(const QString& email);
    void showResetPasswordForm();

    void showAuthForm();
    void showRegForm();
    void showTaskChoiceForm();
    void showSolveForm();

    void onServerConnected();
    void onServerDisconnected();
    void onServerError(const QString& error);
    void onLoginResponse(bool success, const QString& message);
    void onRegisterResponse(bool success, const QString& message);

private:
    void connectSignals();
    void initTestUsers();
    void connectToServer();

    FormAuth* authForm;
    FormReg* regForm;
    FormTaskChoice* taskChoiceForm;
    FormSolve* solveForm;
    MainWindow* mainWindow;
    FormResetPassword* resetPasswordForm;
    bool serverAvailable;

    struct User {
        QString login;
        QString password;
        QString email;
    };
    QMap<QString, User> users;
    QMap<QString, QString> emailToLogin;
};

#endif // FUNCTIONSFORCLIENT_H
