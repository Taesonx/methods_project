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
    //ОСНОВНЫЕ ОБРАБОТЧИКИ ДЕЙСТВИЙ ПОЛЬЗОВАТЕЛЯ
    void processAuth(const QString& login, const QString& password);      // Авторизация
    void processRegister(const QString& login, const QString& password, const QString& email); // Регистрация
    void processTaskSelected(int taskId);                                 // Выбор варианта 29
    void showMainWindow(double a, double b, double c, double d, double e); // Открыть график
    void processResetPassword(const QString& email);                      // Восстановление пароля
    void showResetPasswordForm();                                         // Показать форму восстановления

    //НАВИГАЦИЯ МЕЖДУ ФОРМАМИ
    void showAuthForm();          // Показать авторизацию
    void showRegForm();           // Показать регистрацию
    void showTaskChoiceForm();    // Показать выбор задачи
    void showSolveForm();         // Показать аналитику функции

    //ОБРАБОТЧИКИ ОТВЕТОВ ОТ СЕРВЕРА
    void onServerConnected();                    // Сервер подключился
    void onServerDisconnected();                 // Сервер отключился
    void onServerError(const QString& error);    // Ошибка сервера
    void onLoginResponse(bool success, const QString& message);      // Ответ на авторизацию
    void onRegisterResponse(bool success, const QString& message);   // Ответ на регистрацию
    void onPasswordResetResponse(bool success, const QString& newPassword); // Ответ на восстановление

private:
    void connectSignals();        // Подключение всех сигналов и слотов
    void initTestUsers();         // Создание тестовых пользователей для локального резерва
    void connectToServer();       // Подключение к TCP-серверу

    //УКАЗАТЕЛИ НА ВСЕ ФОРМЫ
    FormAuth* authForm;               // Форма авторизации
    FormReg* regForm;                 // Форма регистрации
    FormTaskChoice* taskChoiceForm;   // Форма выбора задачи
    FormSolve* solveForm;             // Форма с аналитикой функции
    MainWindow* mainWindow;           // Главное окно с графиком и таблицей
    FormResetPassword* resetPasswordForm; // Форма восстановления пароля

    bool serverAvailable;   // Флаг: доступен ли сервер (true — сервер работает)

    //ЛОКАЛЬНОЕ ХРАНИЛИЩЕ ПОЛЬЗОВАТЕЛЕЙ (резерв на случай отключения сервера)
    struct User {
        QString login;
        QString password;
        QString email;
    };
    QMap<QString, User> users;          // Хранилище пользователей: логин → данные
    QMap<QString, QString> emailToLogin;// Хранилище: email → логин (для восстановления пароля)
};

#endif // FUNCTIONSFORCLIENT_H
