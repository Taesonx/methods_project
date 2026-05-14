#ifndef FORMREG_H
#define FORMREG_H

#include <QWidget>

namespace Ui {
class FormReg;
}

class FormReg : public QWidget
{
    Q_OBJECT

public:
    explicit FormReg(QWidget *parent = nullptr);
    ~FormReg();

signals:
    void registerAttempt(const QString& login, const QString& password, const QString& email);
    void backToAuthRequested();

public slots:
    void onRegisterSuccess();
    void onRegisterFailed(const QString& error);

private slots:
    void onRegisterButtonClicked();
    void onBackButtonClicked();
    void onTogglePassword();

private:
    void showError(const QString& title, const QString& message);
    bool isValidEmail(const QString& email);   // Проверка формата email

    Ui::FormReg *ui;
    bool isPasswordVisible;   // Флаг для показа/скрытия пароля
};

#endif // FORMREG_H
