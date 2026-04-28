#ifndef FORMAUTH_H
#define FORMAUTH_H

#include <QWidget>

namespace Ui {
class FormAuth;
}

class FormAuth : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuth(QWidget *parent = nullptr);
    ~FormAuth();

signals:
    void authRequest(const QString& login, const QString& password);
    void registerRequested();
    void forgotPasswordRequested();

public slots:
    void onAuthSuccess();
    void onAuthFailed(const QString& error);

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onForgotButtonClicked();

private:
    void showError(const QString& title, const QString& message);

    Ui::FormAuth *ui;
};

#endif // FORMAUTH_H
