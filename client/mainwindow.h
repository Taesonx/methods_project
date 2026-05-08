#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

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

private slots:
    void onParameterChanged();
    void onCalculateSingle();
    void onBackClicked();

private:
    void setupGraph();
    void calculateRange();
    void updateTable();
    void updateGraph();

    Ui::MainWindow *ui;

    QVector<double> xValues;
    QVector<double> yValues;

    double currentA, currentB, currentC, currentD, currentE;
};

#endif // MAINWINDOW_H
