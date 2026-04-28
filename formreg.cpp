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

void FormReg::showError(const QString& title, const QString& message)
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

bool FormReg::isValidEmail(const QString& email)
{
    if (email.isEmpty()) return false;

    int atPos = email.indexOf('@');
    if (atPos == -1) return false;
    if (atPos == 0 || atPos == email.length() - 1) return false;

    int dotPos = email.indexOf('.', atPos);
    if (dotPos == -1) return false;
    if (dotPos == atPos + 1) return false;
    if (dotPos == email.length() - 1) return false;

    return true;
}

void FormReg::onRegisterButtonClicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text();

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

    if (email.isEmpty()) {
        showError("Ошибка", "Введите email!");
        ui->emailEdit->setFocus();
        return;
    }

    if (!isValidEmail(email)) {
        showError("Ошибка", "Введите корректный email!\nПример: user@example.com");
        ui->emailEdit->setFocus();
        ui->emailEdit->selectAll();
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
    QMessageBox msgBox;
    msgBox.setWindowTitle("✅ УСПЕХ!");
    msgBox.setText("<b style='font-size:16px; color:#2e7d32;'>✨ Регистрация прошла успешно! ✨</b>");
    msgBox.setInformativeText("<b>Теперь вы можете войти в систему</b>\n\nИспользуйте свои логин и пароль для входа.");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setMinimumSize(400, 250);

    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                stop:0 #e8f5e9,"
        "                                stop:1 #c8e6c9);"
        "    border-radius: 10px;"
        "}"
        "QLabel { color: #1b5e20; font-size: 13px; }"
        "QPushButton {"
        "    background-color: #4caf50;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 30px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #388e3c; }"
        );

    msgBox.exec();
    this->hide();
}

void FormReg::onRegisterFailed(const QString& error)
{
    showError("Ошибка регистрации", error);
}
