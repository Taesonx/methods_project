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
    void resetRequested(const QString& email);
    void cancelRequested();

private slots:
    void onResetButtonClicked();
    void onCancelButtonClicked();

public slots:
    void onResetSuccess(const QString& password);
    void onResetFailed(const QString& error);

private:
    Ui::FormResetPassword *ui;
};

#endif // FORMRESETPASSWORD_H
