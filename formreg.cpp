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
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!\nТеперь войдите в систему.");
    this->hide();
}

void FormReg::onRegisterFailed(const QString& error)
{
    QMessageBox::critical(this, "Ошибка регистрации", error);
}
