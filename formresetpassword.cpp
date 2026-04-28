#include "formresetpassword.h"
#include "ui_formresetpassword.h"
#include <QMessageBox>

FormResetPassword::FormResetPassword(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormResetPassword)
{
    ui->setupUi(this);

    connect(ui->resetButton, &QPushButton::clicked,
            this, &FormResetPassword::onResetButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FormResetPassword::onCancelButtonClicked);
}

FormResetPassword::~FormResetPassword()
{
    delete ui;
}

void FormResetPassword::showError(const QString& title, const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("❌ " + title);
    msgBox.setText("<b style='font-size:14px; color:#c62828;'>" + message + "</b>");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setMinimumSize(350, 150);

    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                stop:0 #ffebee,"
        "                                stop:1 #ffcdd2);"
        "    border: 2px solid #c62828;"
        "    border-radius: 10px;"
        "}"
        "QLabel { color: #b71c1c; font-size: 13px; }"
        "QPushButton {"
        "    background-color: #c62828;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 25px;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #b71c1c; }"
        );

    msgBox.exec();
}

void FormResetPassword::onResetButtonClicked()
{
    QString email = ui->emailEdit->text();

    if (email.isEmpty()) {
        showError("Ошибка", "Введите email!");
        ui->emailEdit->setFocus();
        return;
    }

    if (!email.contains('@')) {
        showError("Ошибка", "Введите корректный email!");
        ui->emailEdit->setFocus();
        ui->emailEdit->selectAll();
        return;
    }

    emit resetRequested(email);
}

void FormResetPassword::onCancelButtonClicked()
{
    emit cancelRequested();
}

void FormResetPassword::onResetSuccess(const QString& password)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("🔐 ВОССТАНОВЛЕНИЕ ПАРОЛЯ");
    msgBox.setText("<b style='font-size:16px; color:#1565c0;'>✅ Ваш пароль найден!</b>");
    msgBox.setInformativeText(
        QString("<b style='font-size:24px; color:#d32f2f; background-color:#ffeb3e; "
                "padding:10px; border-radius:8px;'>🔑 %1</b>").arg(password) +
        "<br><br><b>Сохраните его в надёжном месте!</b>"
        );
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setMinimumSize(450, 300);

    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                stop:0 #e3f2fd,"
        "                                stop:1 #bbdefb);"
        "    border-radius: 10px;"
        "}"
        "QLabel { color: #0d47a1; font-size: 13px; }"
        "QPushButton {"
        "    background-color: #1565c0;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 35px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #0d47a1; }"
        );

    msgBox.exec();
    this->hide();
    emit cancelRequested();
}

void FormResetPassword::onResetFailed(const QString& error)
{
    showError("Ошибка", error);
}
