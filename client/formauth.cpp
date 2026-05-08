#include "formauth.h"
#include "ui_formauth.h"
#include <QMessageBox>

FormAuth::FormAuth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuth)
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked,
            this, &FormAuth::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked,
            this, &FormAuth::onRegisterButtonClicked);
    connect(ui->forgotButton, &QPushButton::clicked,
            this, &FormAuth::onForgotButtonClicked);
}

FormAuth::~FormAuth()
{
    delete ui;
}

void FormAuth::onLoginButtonClicked()
{
    emit authRequest(ui->loginEdit->text(), ui->passwordEdit->text());
}

void FormAuth::onRegisterButtonClicked()
{
    emit registerRequested();
}

void FormAuth::onForgotButtonClicked()
{
    emit forgotPasswordRequested();
}

void FormAuth::onAuthSuccess()
{
    this->hide();
}

void FormAuth::onAuthFailed(const QString& error)
{
    QMessageBox::critical(this, "Ошибка авторизации", error);
}
