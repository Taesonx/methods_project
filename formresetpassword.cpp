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

void FormResetPassword::onResetButtonClicked()
{
    QString email = ui->emailEdit->text();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите email!");
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
    QMessageBox::information(this, "Восстановление пароля",
                             QString("Ваш пароль: %1\n\nСохраните его в надёжном месте!").arg(password));
    this->hide();
    emit cancelRequested();  // ← ДОБАВИТЬ ЭТУ СТРОКУ
}

void FormResetPassword::onResetFailed(const QString& error)
{
    QMessageBox::critical(this, "Ошибка", error);
}
