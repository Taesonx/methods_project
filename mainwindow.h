#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setParameters(double a, double b, double c, double d, double e);

signals:
    void backToSolveForm();

private slots:
    void onParameterChangedFirst();
    void onParameterChangedSecond();
    void onParameterChangedThird();
    void onCalculateSingle();
    void onBackToSolveClicked();
    void onBackClicked();

private:
    void showError(const QString& title, const QString& message);
    void setupGraph();
    void zeroAxis();
    void calculateRange();
    void calculateFirstBranch();
    void calculateSecondBranch();
    void calculateThirdBranch();
    void updateTable();
    void updateGraph();
    void sendParametersToServer();
    void sendCalculationToServer(double x, double result);

    Ui::MainWindow *ui;

    QVector<double> xValues1, yValues1;
    QVector<double> xValues2, yValues2;
    QVector<double> xValues3, yValues3;

    double currentA, currentB, currentC, currentD, currentE;
};

#endif // MAINWINDOW_H
