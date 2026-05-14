#include "formauth.h"
#include "ui_formauth.h"
#include <QMessageBox>


 //КОНСТРУКТОР ФОРМЫ АВТОРИЗАЦИИ
FormAuth::FormAuth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuth),
    isPasswordVisible(false)      // Изначально пароль скрыт (точки)
{
    ui->setupUi(this);             // Загружаем UI из .ui файла

    // ===== Подключаем сигналы от кнопок к слотам =====

    // Кнопка "Войти" → onLoginButtonClicked
    connect(ui->loginButton, &QPushButton::clicked,
            this, &FormAuth::onLoginButtonClicked);

    // Кнопка "Регистрация" → onRegisterButtonClicked
    connect(ui->registerButton, &QPushButton::clicked,
            this, &FormAuth::onRegisterButtonClicked);

    // Кнопка "Забыл пароль?" → onForgotButtonClicked
    connect(ui->forgotButton, &QPushButton::clicked,
            this, &FormAuth::onForgotButtonClicked);

    // Кнопка-глаз → onTogglePassword (показать/скрыть пароль)
    connect(ui->togglePasswordButton, &QPushButton::clicked,
            this, &FormAuth::onTogglePassword);
}


 // ДЕСТРУКТОР
FormAuth::~FormAuth()
{
    delete ui;    // Освобождаем память, занятую UI
}


//ОБРАБОТЧИК КНОПКИ-ГЛАЗА
void FormAuth::onTogglePassword()
{
    if (isPasswordVisible) {
        // Скрываем пароль (точечки)
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
        ui->togglePasswordButton->setText("👁️");  // Закрытый глаз
        isPasswordVisible = false;
    } else {
        // Показываем пароль (обычный текст)
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
        ui->togglePasswordButton->setText("🙈");   // Открытый глаз
        isPasswordVisible = true;
    }
}


//ВСПОМОГАТЕЛЬНЫЙ МЕТОД ДЛЯ ПОКАЗА ОШИБОК
void FormAuth::showError(const QString& title, const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("❌ " + title);
    msgBox.setText("<b style='font-size:14px; color:#c62828;'>" + message + "</b>");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


//ОБРАБОТЧИК КНОПКИ "ВОЙТИ"
void FormAuth::onLoginButtonClicked()
{
    QString login = ui->loginEdit->text();        // Получаем логин
    QString password = ui->passwordEdit->text();  // Получаем пароль

    // Валидация: логин не должен быть пустым
    if (login.isEmpty()) {
        showError("Ошибка", "Введите логин!");
        ui->loginEdit->setFocus();
        return;
    }

    // Валидация: пароль не должен быть пустым
    if (password.isEmpty()) {
        showError("Ошибка", "Введите пароль!");
        ui->passwordEdit->setFocus();
        return;
    }

    // Отправляем сигнал в контроллер с логином и паролем
    emit authRequest(login, password);
}

//ОБРАБОТЧИК КНОПКИ "РЕГИСТРАЦИЯ"
void FormAuth::onRegisterButtonClicked()
{
    emit registerRequested();   // Сообщаем контроллеру: показать форму регистрации
}

//ОБРАБОТЧИК КНОПКИ "ЗАБЫЛ ПАРОЛЬ?"
void FormAuth::onForgotButtonClicked()
{
    emit forgotPasswordRequested();   // Сообщаем контроллеру: показать форму восстановления
}

//СЛОТ — ВЫЗЫВАЕТСЯ ПРИ УСПЕШНОЙ АВТОРИЗАЦИИ (вызывается контроллером после получения ответа от сервера)
void FormAuth::onAuthSuccess()
{
    this->hide();   // Прячем форму авторизации (она не закрывается, просто становится невидимой)
    // Контроллер сам покажет следующую форму (выбор задачи)
}

//СЛОТ — ВЫЗЫВАЕТСЯ ПРИ ОШИБКЕ АВТОРИЗАЦИИ (вызывается контроллером)
void FormAuth::onAuthFailed(const QString& error)
{
    showError("Ошибка авторизации", error);   // Показываем сообщение об ошибке
}
