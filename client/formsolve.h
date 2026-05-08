#ifndef FORMSOLVE_H
#define FORMSOLVE_H

#include <QWidget>

namespace Ui {
class FormSolve;
}

class FormSolve : public QWidget
{
    Q_OBJECT

public:
    explicit FormSolve(QWidget *parent = nullptr);
    ~FormSolve();

signals:
    void calculateRequested(double a, double b, double c, double d, double e);
    void backRequested();

private slots:
    void onCalculateButtonClicked();
    void onBackButtonClicked();

private:
    Ui::FormSolve *ui;
};

#endif // FORMSOLVE_H
