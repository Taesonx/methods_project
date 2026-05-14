#ifndef FORMAUTH_H
#define FORMAUTH_H

#include <QWidget>

namespace Ui {
class FormAuth;
}

class FormAuth : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuth(QWidget *parent = nullptr);
    ~FormAuth();

    // Геттеры для получения данных из полей (используются контроллером)
    QString getLogin() const;
    QString getPassword() const;

signals:
    // Сигналы, отправляемые в контроллер
    void authRequest(const QString& login, const QString& password);
    void registerRequested();           // Пользователь хочет зарегистрироваться
    void forgotPasswordRequested();     // Пользователь забыл пароль

public slots:
    // Слоты, вызываемые контроллером
    void onAuthSuccess();               // Успешная авторизация
    void onAuthFailed(const QString& error);  // Ошибка авторизации

private slots:
    // Внутренние обработчики кнопок
    void onLoginButtonClicked();        // Нажата кнопка "Войти"
    void onRegisterButtonClicked();     // Нажата кнопка "Регистрация"
    void onForgotButtonClicked();       // Нажата кнопка "Забыл пароль?"
    void onTogglePassword();            // Нажата кнопка-глаз (показать/скрыть пароль)

private:
    void showError(const QString& title, const QString& message);  // Показать сообщение об ошибке

    Ui::FormAuth *ui;           // Указатель на UI-элементы (из .ui файла)
    bool isPasswordVisible;     // Флаг: виден ли пароль (true — виден, false — скрыт)
};

#endif // FORMAUTH_H
