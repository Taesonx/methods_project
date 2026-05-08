#include "functionsforclient.h"
#include <QDebug>

FunctionsForClient::FunctionsForClient(QObject *parent) : QObject(parent)
{
    authForm = new FormAuth();
    regForm = new FormReg();
    taskChoiceForm = new FormTaskChoice();
    solveForm = new FormSolve();
    mainWindow = new MainWindow();
    resetPasswordForm = new FormResetPassword();

    initTestUsers();
    connectSignals();
    showAuthForm();
}

FunctionsForClient::~FunctionsForClient()
{
    delete authForm;
    delete regForm;
    delete taskChoiceForm;
    delete solveForm;
    delete mainWindow;
    delete resetPasswordForm;
}

void FunctionsForClient::initTestUsers()
{
    User testUser;
    testUser.login = "user";
    testUser.password = "123";
    testUser.email = "user@example.com";
    users["user"] = testUser;
    emailToLogin["user@example.com"] = "user";
}

void FunctionsForClient::connectSignals()
{
    // FormAuth
    connect(authForm, &FormAuth::authRequest,
            this, &FunctionsForClient::processAuth);
    connect(authForm, &FormAuth::registerRequested,
            this, &FunctionsForClient::showRegForm);
    connect(authForm, &FormAuth::forgotPasswordRequested,
            this, &FunctionsForClient::showResetPasswordForm);

    // FormReg
    connect(regForm, &FormReg::registerAttempt,
            this, &FunctionsForClient::processRegister);
    connect(regForm, &FormReg::backToAuthRequested,
            this, &FunctionsForClient::showAuthForm);

    // FormTaskChoice
    connect(taskChoiceForm, &FormTaskChoice::taskSelected,
            this, &FunctionsForClient::processTaskSelected);

    // FormSolve
    connect(solveForm, &FormSolve::calculateRequested,
            this, &FunctionsForClient::showMainWindow);
    connect(solveForm, &FormSolve::backRequested,
            this, &FunctionsForClient::showTaskChoiceForm);

    // MainWindow
    connect(mainWindow, &MainWindow::destroyed,
            this, &FunctionsForClient::showTaskChoiceForm);

    // FormResetPassword
    connect(resetPasswordForm, &FormResetPassword::resetRequested,
            this, &FunctionsForClient::processResetPassword);
    connect(resetPasswordForm, &FormResetPassword::cancelRequested,
            this, &FunctionsForClient::showAuthForm);
}

void FunctionsForClient::processAuth(const QString& login, const QString& password)
{
    if (users.contains(login) && users[login].password == password) {
        authForm->onAuthSuccess();
        showTaskChoiceForm();
    } else {
        authForm->onAuthFailed("Неверный логин или пароль");
    }
}

void FunctionsForClient::processRegister(const QString& login, const QString& password, const QString& email)
{
    if (users.contains(login)) {
        regForm->onRegisterFailed("Пользователь уже существует");
        return;
    }

    User newUser;
    newUser.login = login;
    newUser.password = password;
    newUser.email = email;
    users[login] = newUser;
    emailToLogin[email] = login;

    regForm->onRegisterSuccess();
    showAuthForm();
}

void FunctionsForClient::processTaskSelected(int taskId)
{
    if (taskId == 29) {
        showSolveForm();
    }
}

void FunctionsForClient::processResetPassword(const QString& email)
{
    if (emailToLogin.contains(email)) {
        QString login = emailToLogin[email];
        QString password = users[login].password;
        resetPasswordForm->onResetSuccess(password);
    } else {
        resetPasswordForm->onResetFailed("Пользователь с таким email не найден!");
    }
}

void FunctionsForClient::showMainWindow(double a, double b, double c, double d, double e)
{
    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    resetPasswordForm->hide();

    mainWindow->setParameters(a, b, c, d, e);
    mainWindow->show();
}

void FunctionsForClient::showAuthForm()
{
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    authForm->show();
}

void FunctionsForClient::showRegForm()
{
    authForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    regForm->show();
}

void FunctionsForClient::showTaskChoiceForm()
{
    authForm->hide();
    regForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    taskChoiceForm->show();
}

void FunctionsForClient::showSolveForm()
{
    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    solveForm->show();
}

void FunctionsForClient::showResetPasswordForm()
{
    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->show();
}
