#ifndef FORMRESETPASSWORD_H
#define FORMRESETPASSWORD_H

#include <QWidget>

namespace Ui {
class FormResetPassword;
}

class FormResetPassword : public QWidget
{
    Q_OBJECT

public:
    explicit FormResetPassword(QWidget *parent = nullptr);
    ~FormResetPassword();

signals:
    void resetRequested(const QString& email);   // Запрос на восстановление
    void cancelRequested();                      // Отмена

private slots:
    void onResetButtonClicked();     // Нажата кнопка "Восстановить"
    void onCancelButtonClicked();    // Нажата кнопка "Отмена"

public slots:
    void onResetSuccess(const QString& password);   // Успех: показать пароль
    void onResetFailed(const QString& error);       // Ошибка

private:
    void showError(const QString& title, const QString& message);

    Ui::FormResetPassword *ui;
};

#endif // FORMRESETPASSWORD_H
