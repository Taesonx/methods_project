#include "functionsforclient.h"
#include <QDebug>

FunctionsForClient::FunctionsForClient(QObject *parent) : QObject(parent)
{
    authForm = new FormAuth();
    regForm = new FormReg();
    taskChoiceForm = new FormTaskChoice();
    solveForm = new FormSolve();
    mainWindow = new MainWindow();

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
}

void FunctionsForClient::initTestUsers()
{
    User testUser;
    testUser.login = "user";
    testUser.password = "123";
    testUser.email = "user@example.com";
    users["user"] = testUser;
}

void FunctionsForClient::connectSignals()
{
    connect(authForm, &FormAuth::authRequest,
            this, &FunctionsForClient::processAuth);
    connect(authForm, &FormAuth::registerRequested,
            this, &FunctionsForClient::showRegForm);

    connect(regForm, &FormReg::registerAttempt,
            this, &FunctionsForClient::processRegister);
    connect(regForm, &FormReg::backToAuthRequested,
            this, &FunctionsForClient::showAuthForm);

    connect(taskChoiceForm, &FormTaskChoice::taskSelected,
            this, &FunctionsForClient::processTaskSelected);

    connect(solveForm, &FormSolve::calculateRequested,
            this, &FunctionsForClient::showMainWindow);
    connect(solveForm, &FormSolve::backRequested,
            this, &FunctionsForClient::showTaskChoiceForm);

    connect(mainWindow, &MainWindow::destroyed,
            this, &FunctionsForClient::showTaskChoiceForm);
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

    regForm->onRegisterSuccess();
    showAuthForm();
}

void FunctionsForClient::processTaskSelected(int taskId)
{
    if (taskId == 29) {
        showSolveForm();
    }
}

void FunctionsForClient::showMainWindow(double a, double b, double c, double d, double e)
{
    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();

    mainWindow->setParameters(a, b, c, d, e);
    mainWindow->show();
}

void FunctionsForClient::showAuthForm()
{
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    authForm->show();
}

void FunctionsForClient::showRegForm()
{
    authForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    regForm->show();
}

void FunctionsForClient::showTaskChoiceForm()
{
    authForm->hide();
    regForm->hide();
    solveForm->hide();
    mainWindow->hide();
    taskChoiceForm->show();
}

void FunctionsForClient::showSolveForm()
{
    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    mainWindow->hide();
    solveForm->show();
}
