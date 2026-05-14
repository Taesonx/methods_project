#include "formresetpassword.h"
#include "ui_formresetpassword.h"
#include <QMessageBox>


//КОНСТРУКТОР
FormResetPassword::FormResetPassword(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormResetPassword)
{
    ui->setupUi(this);

    // Подключаем кнопки
    connect(ui->resetButton, &QPushButton::clicked,
            this, &FormResetPassword::onResetButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FormResetPassword::onCancelButtonClicked);
}


//ДЕСТРУКТОР
FormResetPassword::~FormResetPassword()
{
    delete ui;
}


//ОБРАБОТЧИК КНОПКИ "ВОССТАНОВИТЬ"
void FormResetPassword::onResetButtonClicked()
{
    QString email = ui->emailEdit->text();

    // Проверка: email не пустой
    if (email.isEmpty()) {
        showError("Ошибка", "Введите email!");
        ui->emailEdit->setFocus();
        return;
    }

    // Простая проверка: email должен содержать @
    if (!email.contains('@')) {
        showError("Ошибка", "Введите корректный email!");
        ui->emailEdit->setFocus();
        ui->emailEdit->selectAll();
        return;
    }

    // Отправляем сигнал в контроллер
    emit resetRequested(email);
}


//ОБРАБОТЧИК КНОПКИ "ОТМЕНА"
void FormResetPassword::onCancelButtonClicked()
{
    emit cancelRequested();   // Возвращаемся к авторизации
}


//СЛОТ — УСПЕШНОЕ ВОССТАНОВЛЕНИЕ (ПАРОЛЬ НАЙДЕН)
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
    msgBox.exec();

    this->hide();                // Закрываем форму восстановления
    emit cancelRequested();      // Возвращаемся к авторизации
}


//СЛОТ — ОШИБКА ВОССТАНОВЛЕНИЯ
void FormResetPassword::onResetFailed(const QString& error)
{
    showError("Ошибка", error);
}


//ВСПОМОГАТЕЛЬНЫЙ МЕТОД ДЛЯ ПОКАЗА ОШИБОК
void FormResetPassword::showError(const QString& title, const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("❌ " + title);
    msgBox.setText("<b style='font-size:14px; color:#c62828;'>" + message + "</b>");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
