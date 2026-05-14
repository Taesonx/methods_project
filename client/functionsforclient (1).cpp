#include "functionsforclient.h"
#include "tcpclient.h"
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>


// ============================================================
// КОНСТРУКТОР И ДЕСТРУКТОР
// ============================================================

FunctionsForClient::FunctionsForClient(QObject *parent) : QObject(parent)
{
    qDebug() << "=== FunctionsForClient: Инициализация контроллера ===";

    // 1. Создаем все формы
    authForm = new FormAuth();
    regForm = new FormReg();
    taskChoiceForm = new FormTaskChoice();
    solveForm = new FormSolve();
    mainWindow = new MainWindow();
    resetPasswordForm = new FormResetPassword();

    // 2. Инициализируем флаг доступности сервера
    serverAvailable = false;

    qDebug() << "Формы созданы:";
    qDebug() << "  - FormAuth (авторизация)";
    qDebug() << "  - FormReg (регистрация)";
    qDebug() << "  - FormTaskChoice (выбор задачи)";
    qDebug() << "  - FormSolve (решение)";
    qDebug() << "  - MainWindow (график и таблица)";
    qDebug() << "  - FormResetPassword (восстановление пароля)";

    // 3. Инициализируем тестовых пользователей (локальный резерв)
    initTestUsers();

    // 4. Подключаем сигналы и слоты
    connectSignals();

    // 5. Подключаемся к серверу
    connectToServer();

    // 6. Показываем начальную форму (авторизация)
    showAuthForm();

    qDebug() << "=== Контроллер готов. Ожидание действий пользователя ===";
}

FunctionsForClient::~FunctionsForClient()
{
    qDebug() << "=== FunctionsForClient: Завершение работы ===";

    // Удаляем формы
    delete authForm;
    delete regForm;
    delete taskChoiceForm;
    delete solveForm;
    delete mainWindow;
    delete resetPasswordForm;

    // Отключаемся от сервера
    TcpClient::getInstance().disconnectFromServer();

    qDebug() << "Все формы удалены";
}


// ============================================================
// ПОДКЛЮЧЕНИЕ К СЕРВЕРУ
// ============================================================

void FunctionsForClient::connectToServer()
{
    qDebug() << "Подключение к серверу 127.0.0.1:8080...";
    TcpClient& client = TcpClient::getInstance();
    client.connectToServer("127.0.0.1", 8080);
}


// ============================================================
// ИНИЦИАЛИЗАЦИЯ ТЕСТОВЫХ ПОЛЬЗОВАТЕЛЕЙ (ЛОКАЛЬНЫЙ РЕЗЕРВ)
// ============================================================

void FunctionsForClient::initTestUsers()
{
    qDebug() << "Инициализация тестовых пользователей (локальный резерв)...";

    // Тестовый пользователь 1
    User testUser1;
    testUser1.login = "user";
    testUser1.password = "123";
    testUser1.email = "user@example.com";
    users["user"] = testUser1;
    emailToLogin["user@example.com"] = "user";

    // Тестовый пользователь 2
    User testUser2;
    testUser2.login = "admin";
    testUser2.password = "admin123";
    testUser2.email = "admin@example.com";
    users["admin"] = testUser2;
    emailToLogin["admin@example.com"] = "admin";

    // Тестовый пользователь 3
    User testUser3;
    testUser3.login = "test";
    testUser3.password = "test";
    testUser3.email = "test@example.com";
    users["test"] = testUser3;
    emailToLogin["test@example.com"] = "test";

    qDebug() << "Добавлены тестовые пользователи:";
    qDebug() << "  - user / 123 / user@example.com";
    qDebug() << "  - admin / admin123 / admin@example.com";
    qDebug() << "  - test / test / test@example.com";
}


// ============================================================
// ПОДКЛЮЧЕНИЕ СИГНАЛОВ
// ============================================================

void FunctionsForClient::connectSignals()
{
    qDebug() << "Подключение сигналов...";

    // ---------- ФОРМА АВТОРИЗАЦИИ ----------
    connect(authForm, &FormAuth::authRequest,
            this, &FunctionsForClient::processAuth);
    connect(authForm, &FormAuth::registerRequested,
            this, &FunctionsForClient::showRegForm);
    connect(authForm, &FormAuth::forgotPasswordRequested,
            this, &FunctionsForClient::showResetPasswordForm);

    qDebug() << "  - FormAuth → processAuth";
    qDebug() << "  - FormAuth → showRegForm";
    qDebug() << "  - FormAuth → showResetPasswordForm";

    // ---------- ФОРМА РЕГИСТРАЦИИ ----------
    connect(regForm, &FormReg::registerAttempt,
            this, &FunctionsForClient::processRegister);
    connect(regForm, &FormReg::backToAuthRequested,
            this, &FunctionsForClient::showAuthForm);

    qDebug() << "  - FormReg → processRegister";
    qDebug() << "  - FormReg → showAuthForm";

    // ---------- ФОРМА ВЫБОРА ЗАДАЧИ ----------
    connect(taskChoiceForm, &FormTaskChoice::taskSelected,
            this, &FunctionsForClient::processTaskSelected);

    qDebug() << "  - FormTaskChoice → processTaskSelected";

    // ---------- ФОРМА РЕШЕНИЯ ----------
    connect(solveForm, &FormSolve::calculateRequested,
            this, &FunctionsForClient::showMainWindow);
    connect(solveForm, &FormSolve::backRequested,
            this, &FunctionsForClient::showTaskChoiceForm);

    qDebug() << "  - FormSolve → showMainWindow";
    qDebug() << "  - FormSolve → showTaskChoiceForm";

    // ---------- ГЛАВНОЕ ОКНО (MainWindow) ----------
    // Кнопка "Назад" → возврат на FormSolve
    connect(mainWindow, &MainWindow::backToSolveForm, this, &FunctionsForClient::showSolveForm);
    // Кнопка "Завершить" или закрытие окна → возврат на FormTaskChoice
    connect(mainWindow, &MainWindow::destroyed, this, &FunctionsForClient::showTaskChoiceForm);

    qDebug() << "  - MainWindow → showSolveForm (кнопка Назад)";
    qDebug() << "  - MainWindow → showTaskChoiceForm (кнопка Завершить)";

    // ---------- ФОРМА ВОССТАНОВЛЕНИЯ ПАРОЛЯ ----------
    connect(resetPasswordForm, &FormResetPassword::resetRequested,
            this, &FunctionsForClient::processResetPassword);
    connect(resetPasswordForm, &FormResetPassword::cancelRequested,
            this, &FunctionsForClient::showAuthForm);

    qDebug() << "  - FormResetPassword → processResetPassword";
    qDebug() << "  - FormResetPassword → showAuthForm";

    // ---------- TCP CLIENT ----------
    TcpClient& client = TcpClient::getInstance();
    connect(&client, &TcpClient::connected,
            this, &FunctionsForClient::onServerConnected);
    connect(&client, &TcpClient::disconnected,
            this, &FunctionsForClient::onServerDisconnected);
    connect(&client, &TcpClient::errorOccurred,
            this, &FunctionsForClient::onServerError);
    connect(&client, &TcpClient::loginResponse,
            this, &FunctionsForClient::onLoginResponse);
    connect(&client, &TcpClient::registerResponse,
            this, &FunctionsForClient::onRegisterResponse);
    connect(&client, &TcpClient::passwordResetResponse,
            this, &FunctionsForClient::onPasswordResetResponse);

    qDebug() << "  - TcpClient → onServerConnected";
    qDebug() << "  - TcpClient → onServerDisconnected";
    qDebug() << "  - TcpClient → onServerError";
    qDebug() << "  - TcpClient → onLoginResponse";
    qDebug() << "  - TcpClient → onRegisterResponse";
    qDebug() << "  - TcpClient → onPasswordResetResponse";

    qDebug() << "Все сигналы подключены!";
}


// ============================================================
// ОБРАБОТЧИКИ СОСТОЯНИЯ СЕРВЕРА
// ============================================================

void FunctionsForClient::onServerConnected()
{
    qDebug() << "Сервер доступен!";
    serverAvailable = true;
}

void FunctionsForClient::onServerDisconnected()
{
    qDebug() << "Сервер недоступен! Используем локальную проверку.";
    serverAvailable = false;
}

void FunctionsForClient::onServerError(const QString& error)
{
    qDebug() << "Ошибка сервера:" << error;
    serverAvailable = false;
}


// ============================================================
// ОБРАБОТЧИКИ ОТВЕТОВ ОТ СЕРВЕРА
// ============================================================

void FunctionsForClient::onLoginResponse(bool success, const QString& message)
{
    qDebug() << "Ответ сервера на авторизацию: success=" << success << "message=" << message;

    if (success) {
        authForm->onAuthSuccess();
        showTaskChoiceForm();
    } else {
        authForm->onAuthFailed(message);
    }
}

void FunctionsForClient::onRegisterResponse(bool success, const QString& message)
{
    qDebug() << "Ответ сервера на регистрацию: success=" << success << "message=" << message;

    if (success) {
        regForm->onRegisterSuccess();
        showAuthForm();
    } else {
        regForm->onRegisterFailed(message);
    }
}

void FunctionsForClient::onPasswordResetResponse(bool success, const QString& newPassword)
{
    qDebug() << "Ответ сервера на восстановление пароля: success=" << success;

    if (success) {
        resetPasswordForm->onResetSuccess(newPassword);
    } else {
        resetPasswordForm->onResetFailed(newPassword);
    }
}


// ============================================================
// ОБРАБОТЧИК АВТОРИЗАЦИИ (С ПОДДЕРЖКОЙ TCP)
// ============================================================

void FunctionsForClient::processAuth(const QString& login, const QString& password)
{
    qDebug() << "========================================";
    qDebug() << "ОБРАБОТКА АВТОРИЗАЦИИ";
    qDebug() << "  Логин:" << login;
    qDebug() << "  Пароль:" << password;
    qDebug() << "  Сервер доступен:" << serverAvailable;

    if (serverAvailable) {
        // Отправляем запрос на сервер
        qDebug() << "  Отправка запроса на сервер /login";
        QJsonObject data;
        data["login"] = login;
        data["password"] = password;
        TcpClient::getInstance().sendRequest("/login", data);
    } else {
        // Используем локальную проверку
        qDebug() << "  Используем локальную проверку";
        if (users.contains(login) && users[login].password == password) {
            qDebug() << "  Локальная проверка: УСПЕШНО";
            authForm->onAuthSuccess();
            showTaskChoiceForm();
        } else {
            qDebug() << "  Локальная проверка: НЕ УДАЛАСЬ";
            authForm->onAuthFailed("Неверный логин или пароль (локальная проверка)");
        }
    }

    qDebug() << "========================================";
}


// ============================================================
// ОБРАБОТЧИК РЕГИСТРАЦИИ (С ПОДДЕРЖКОЙ TCP)
// ============================================================

void FunctionsForClient::processRegister(const QString& login, const QString& password, const QString& email)
{
    qDebug() << "========================================";
    qDebug() << "ОБРАБОТКА РЕГИСТРАЦИИ";
    qDebug() << "  Логин:" << login;
    qDebug() << "  Пароль:" << password;
    qDebug() << "  Email:" << email;
    qDebug() << "  Сервер доступен:" << serverAvailable;

    if (serverAvailable) {
        // Отправляем запрос на сервер
        qDebug() << "  Отправка запроса на сервер /register";
        QJsonObject data;
        data["login"] = login;
        data["password"] = password;
        data["email"] = email;
        TcpClient::getInstance().sendRequest("/register", data);
    } else {
        // Используем локальную проверку
        qDebug() << "  Используем локальную проверку";
        if (users.contains(login)) {
            qDebug() << "  Локальная проверка: ПОЛЬЗОВАТЕЛЬ УЖЕ СУЩЕСТВУЕТ";
            regForm->onRegisterFailed("Пользователь уже существует");
            return;
        }

        User newUser;
        newUser.login = login;
        newUser.password = password;
        newUser.email = email;
        users[login] = newUser;
        emailToLogin[email] = login;

        qDebug() << "  Локальная проверка: РЕГИСТРАЦИЯ УСПЕШНА";
        regForm->onRegisterSuccess();
        showAuthForm();
    }

    qDebug() << "========================================";
}


// ============================================================
// ОБРАБОТЧИК ВОССТАНОВЛЕНИЯ ПАРОЛЯ
// ============================================================

void FunctionsForClient::processResetPassword(const QString& email)
{
    qDebug() << "========================================";
    qDebug() << "ВОССТАНОВЛЕНИЕ ПАРОЛЯ";
    qDebug() << "  Email:" << email;
    qDebug() << "  Сервер доступен:" << serverAvailable;

    if (serverAvailable) {
        // Отправляем запрос на сервер
        qDebug() << "  Отправка запроса на сервер /reset";
        QJsonObject data;
        data["email"] = email;
        TcpClient::getInstance().sendRequest("/reset", data);
    } else {
        // Используем локальную проверку
        qDebug() << "  Используем локальную проверку";
        if (emailToLogin.contains(email)) {
            QString login = emailToLogin[email];
            QString password = users[login].password;
            qDebug() << "  Найден пользователь:" << login << "пароль:" << password;
            resetPasswordForm->onResetSuccess(password);
        } else {
            qDebug() << "  Email не найден!";
            resetPasswordForm->onResetFailed("Пользователь с таким email не найден!");
        }
    }

    qDebug() << "========================================";
}


// ============================================================
// ОБРАБОТЧИК ВЫБОРА ЗАДАЧИ
// ============================================================

void FunctionsForClient::processTaskSelected(int taskId)
{
    qDebug() << "========================================";
    qDebug() << "ВЫБОР ЗАДАЧИ";
    qDebug() << "  Выбран вариант:" << taskId;

    if (taskId == 29) {
        qDebug() << "  Открываем форму решения задачи (Вариант 29)";
        showSolveForm();
    } else {
        qDebug() << "  Неизвестный вариант!";
    }

    qDebug() << "========================================";
}


// ============================================================
// ПОКАЗ ГЛАВНОГО ОКНА (ГРАФИК И ТАБЛИЦА)
// ============================================================

void FunctionsForClient::showMainWindow(double a, double b, double c, double d, double e)
{
    qDebug() << "Навигация: Показываем ГЛАВНОЕ ОКНО с графиком";
    qDebug() << "  Параметры: a=" << a << ", b=" << b << ", c=" << c
             << ", d=" << d << ", e=" << e;

    // Скрываем все формы
    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    resetPasswordForm->hide();

    // Передаём параметры и показываем главное окно
    mainWindow->setParameters(a, b, c, d, e);
    mainWindow->show();
}


// ============================================================
// НАВИГАЦИЯ МЕЖДУ ФОРМАМИ
// ============================================================

void FunctionsForClient::showAuthForm()
{
    qDebug() << "Навигация: Показываем форму АВТОРИЗАЦИИ";

    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    authForm->show();
}

void FunctionsForClient::showRegForm()
{
    qDebug() << "Навигация: Показываем форму РЕГИСТРАЦИИ";

    authForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    regForm->show();
}

void FunctionsForClient::showTaskChoiceForm()
{
    qDebug() << "Навигация: Показываем форму ВЫБОРА ЗАДАЧИ";

    authForm->hide();
    regForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    taskChoiceForm->show();
}

void FunctionsForClient::showSolveForm()
{
    qDebug() << "Навигация: Показываем форму РЕШЕНИЯ ЗАДАЧИ";

    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    mainWindow->hide();
    resetPasswordForm->hide();
    solveForm->show();
}

void FunctionsForClient::showResetPasswordForm()
{
    qDebug() << "Навигация: Показываем форму ВОССТАНОВЛЕНИЯ ПАРОЛЯ";

    authForm->hide();
    regForm->hide();
    taskChoiceForm->hide();
    solveForm->hide();
    mainWindow->hide();
    resetPasswordForm->show();
}
