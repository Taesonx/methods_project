#include "formreg.h"
#include "ui_formreg.h"
#include <QMessageBox>

FormReg::FormReg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReg)
{
    ui->setupUi(this);

    connect(ui->registerButton, &QPushButton::clicked,
            this, &FormReg::onRegisterButtonClicked);
    connect(ui->backButton, &QPushButton::clicked,
            this, &FormReg::onBackButtonClicked);
}

FormReg::~FormReg()
{
    delete ui;
}

void FormReg::onRegisterButtonClicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text();

    if (login.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин!");
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите пароль!");
        return;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите email!");
        return;
    }

    emit registerAttempt(login, password, email);
}

void FormReg::onBackButtonClicked()
{
    emit backToAuthRequested();
}

void FormReg::onRegisterSuccess()
{
    // Создаём кастомное сообщение об успехе
    QMessageBox msgBox;
    msgBox.setWindowTitle("✅ УСПЕХ!");

    // Основной текст с HTML форматированием
    msgBox.setText("<b style='font-size:16px; color:#2e7d32;'>✨ Регистрация прошла успешно! ✨</b>");
    msgBox.setInformativeText("<b>Теперь вы можете войти в систему</b>\n\n"
                              "Используйте свои логин и пароль для входа.");

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    // Увеличиваем размер окна сообщения
    msgBox.setMinimumSize(400, 250);

    // Стилизация окна сообщения
    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                stop:0 #e8f5e9,"
        "                                stop:1 #c8e6c9);"
        "    border-radius: 10px;"
        "}"
        "QLabel {"
        "    color: #1b5e20;"
        "    font-size: 13px;"
        "}"
        "QPushButton {"
        "    background-color: #4caf50;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 30px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #388e3c;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2e7d32;"
        "}"
        );

    msgBox.exec();
    this->hide();
}

void FormReg::onRegisterFailed(const QString& error)
{
    QMessageBox::critical(this, "Ошибка регистрации", error);
}
