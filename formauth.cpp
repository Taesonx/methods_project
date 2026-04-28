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

void FormAuth::showError(const QString& title, const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("❌ " + title);
    msgBox.setText("<b style='font-size:14px; color:#c62828;'>" + message + "</b>");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setMinimumSize(350, 150);

    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                stop:0 #ffebee,"
        "                                stop:1 #ffcdd2);"
        "    border: 2px solid #c62828;"
        "    border-radius: 10px;"
        "}"
        "QLabel {"
        "    color: #b71c1c;"
        "    font-size: 13px;"
        "}"
        "QPushButton {"
        "    background-color: #c62828;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 25px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #b71c1c;"
        "}"
        );

    msgBox.exec();
}

void FormAuth::onLoginButtonClicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    if (login.isEmpty()) {
        showError("Ошибка", "Введите логин!");
        ui->loginEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        showError("Ошибка", "Введите пароль!");
        ui->passwordEdit->setFocus();
        return;
    }

    emit authRequest(login, password);
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
    showError("Ошибка авторизации", error);
}
