#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mathfunction.h"
#include "tcpclient.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QTableWidgetItem>
#include <QJsonObject>
#include <algorithm>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentA = 0.0;
    currentB = 0.0;
    currentC = 0.0;
    currentD = 0.0;
    currentE = 0.0;

    ui->aSpinBox->setValue(currentA);
    ui->bSpinBox->setValue(currentB);
    ui->cSpinBox->setValue(currentC);
    ui->dSpinBox->setValue(currentD);
    ui->eSpinBox->setValue(currentE);

    ui->aSlider->setValue(currentA * 10);
    ui->bSlider->setValue(currentB * 10);
    ui->cSlider->setValue(currentC * 10);
    ui->dSlider->setValue(currentD * 10);
    ui->eSlider->setValue(currentE * 10);

    connect(ui->aSlider, &QSlider::valueChanged, [this](int val) {
        ui->aSpinBox->setValue(val / 10.0);
    });
    connect(ui->bSlider, &QSlider::valueChanged, [this](int val) {
        ui->bSpinBox->setValue(val / 10.0);
    });
    connect(ui->cSlider, &QSlider::valueChanged, [this](int val) {
        ui->cSpinBox->setValue(val / 10.0);
    });
    connect(ui->dSlider, &QSlider::valueChanged, [this](int val) {
        ui->dSpinBox->setValue(val / 10.0);
    });
    connect(ui->eSlider, &QSlider::valueChanged, [this](int val) {
        ui->eSpinBox->setValue(val / 10.0);
    });

    connect(ui->aSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChangedFirst);
    connect(ui->bSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChangedSecond);
    connect(ui->cSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChangedThird);
    connect(ui->dSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChangedThird);
    connect(ui->eSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChangedThird);

    connect(ui->calculateSingleButton, &QPushButton::clicked,
            this, &MainWindow::onCalculateSingle);
    connect(ui->backToSolveButton, &QPushButton::clicked,
            this, &MainWindow::onBackToSolveClicked);
    connect(ui->backButton, &QPushButton::clicked,
            this, &MainWindow::onBackClicked);

    setupGraph();
    calculateRange();
    updateTable();
    updateGraph();
    sendParametersToServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showError(const QString& title, const QString& message)
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

void MainWindow::setParameters(double a, double b, double c, double d, double e)
{
    currentA = a;
    currentB = b;
    currentC = c;
    currentD = d;
    currentE = e;

    ui->aSpinBox->setValue(a);
    ui->bSpinBox->setValue(b);
    ui->cSpinBox->setValue(c);
    ui->dSpinBox->setValue(d);
    ui->eSpinBox->setValue(e);

    calculateRange();
    updateTable();
    updateGraph();
    sendParametersToServer();
}

void MainWindow::setupGraph()
{
    QChart *chart = new QChart();
    chart->setTitle("Графики функции f(x)");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("x");
    axisX->setLabelFormat("%.1f");
    axisX->setRange(-20, 20);
    axisX->setLinePen(QPen(Qt::black, 2));
    axisX->setGridLinePen(QPen(Qt::gray, 1));
    axisX->setTitleBrush(QBrush(Qt::black));
    axisX->setLabelsColor(Qt::black);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("f(x)");
    axisY->setLabelFormat("%.2f");
    axisY->setRange(-100, 100);
    axisY->setLinePen(QPen(Qt::black, 2));
    axisY->setGridLinePen(QPen(Qt::gray, 1));
    axisY->setTitleBrush(QBrush(Qt::black));
    axisY->setLabelsColor(Qt::black);
    chart->addAxis(axisY, Qt::AlignLeft);

    ui->graphView->setChart(chart);
    ui->graphView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::zeroAxis()
{
    QChart *chart = ui->graphView->chart();

    QList<QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries *series : seriesList) {
        if (series->name() == "x=0 line") {
            chart->removeSeries(series);
            delete series;
        }
    }

    QLineSeries *zeroLine = new QLineSeries();
    zeroLine->setName("x=0 line");
    zeroLine->setColor(Qt::black);
    zeroLine->setPen(QPen(Qt::black, 1.5, Qt::DashLine));

    double yMin = -100;
    double yMax = 100;

    zeroLine->append(0, yMin);
    zeroLine->append(0, yMax);

    chart->addSeries(zeroLine);

    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis *axis : axes) {
        zeroLine->attachAxis(axis);
    }
}

void MainWindow::calculateRange()
{
    calculateFirstBranch();
    calculateSecondBranch();
    calculateThirdBranch();
}

void MainWindow::calculateFirstBranch()
{
    xValues1.clear();
    yValues1.clear();

    for (double x = -20; x <= 20; x += 0.05) {
        double y;
        if (x >= 0) {
            y = currentA * sqrt(x);
        } else {
            y = NAN;
        }
        xValues1.append(x);
        yValues1.append(y);
    }
}

void MainWindow::calculateSecondBranch()
{
    xValues2.clear();
    yValues2.clear();

    for (double x = -20; x <= 20; x += 0.05) {
        double y;
        if (fabs(x) < 0.001) {
            y = NAN;
        } else {
            y = currentB / x;
        }
        xValues2.append(x);
        yValues2.append(y);
    }
}

void MainWindow::calculateThirdBranch()
{
    xValues3.clear();
    yValues3.clear();

    for (double x = -20; x <= 20; x += 0.05) {
        double y = currentC * x * x - currentD * x + currentE;
        xValues3.append(x);
        yValues3.append(y);
    }
}

void MainWindow::updateTable()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"x", "a·√x", "b/x", "c·x² - d·x + e"});

    for (double x = -20; x <= 20; x += 1.0) {
        // Красная ветка: a·√x
        double y1 = 0;
        if (x >= 0) {
            y1 = currentA * sqrt(x);
        }

        // Зелёная ветка: b/x
        double y2 = 0;
        if (fabs(x) > 0.001) {
            y2 = currentB / x;
        }

        // Синяя ветка: квадратичная
        double y3 = currentC * x * x - currentD * x + currentE;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(x, 'f', 1)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(y1, 'f', 6)));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(y2, 'f', 6)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(y3, 'f', 6)));
    }

    ui->tableWidget->resizeColumnToContents(0);
    ui->tableWidget->resizeColumnToContents(1);
    ui->tableWidget->resizeColumnToContents(2);
    ui->tableWidget->resizeColumnToContents(3);
}

void MainWindow::updateGraph()
{
    QChart *chart = ui->graphView->chart();

    QList<QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries *series : seriesList) {
        if (series->name() != "x=0 line") {
            chart->removeSeries(series);
            delete series;
        }
    }

    // 1. КРАСНЫЙ: a·√x
    QLineSeries *series1 = new QLineSeries();
    series1->setName(QString("a·√x (a=%1)").arg(currentA));
    series1->setColor(Qt::red);
    series1->setPen(QPen(Qt::red, 2));
    for (int i = 0; i < xValues1.size(); i++) {
        if (!std::isnan(yValues1[i])) {
            series1->append(xValues1[i], yValues1[i]);
        }
    }
    chart->addSeries(series1);

    // 2. ЗЕЛЁНЫЙ: b/x
    QLineSeries *series2 = new QLineSeries();
    series2->setName(QString("b/x (b=%1)").arg(currentB));
    series2->setColor(Qt::green);
    series2->setPen(QPen(Qt::green, 2));
    for (int i = 0; i < xValues2.size(); i++) {
        if (!std::isnan(yValues2[i])) {
            series2->append(xValues2[i], yValues2[i]);
        }
    }
    chart->addSeries(series2);

    // 3. СИНИЙ: квадратичный
    QLineSeries *series3 = new QLineSeries();
    series3->setName(QString("c·x² - d·x + e (c=%1, d=%2, e=%3)")
                         .arg(currentC).arg(currentD).arg(currentE));
    series3->setColor(Qt::blue);
    series3->setPen(QPen(Qt::blue, 3));
    for (int i = 0; i < xValues3.size(); i++) {
        series3->append(xValues3[i], yValues3[i]);
    }
    chart->addSeries(series3);

    // Обновляем оси
    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis *axis : axes) {
        chart->removeAxis(axis);
        delete axis;
    }

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("x");
    axisX->setLabelFormat("%.1f");
    axisX->setRange(-20, 20);
    axisX->setLinePen(QPen(Qt::black, 2));
    axisX->setGridLinePen(QPen(Qt::gray, 1));
    axisX->setTitleBrush(QBrush(Qt::black));
    axisX->setLabelsColor(Qt::black);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("f(x)");
    axisY->setLabelFormat("%.2f");
    axisY->setRange(-100, 100);
    axisY->setLinePen(QPen(Qt::black, 2));
    axisY->setGridLinePen(QPen(Qt::gray, 1));
    axisY->setTitleBrush(QBrush(Qt::black));
    axisY->setLabelsColor(Qt::black);
    chart->addAxis(axisY, Qt::AlignLeft);

    series1->attachAxis(axisX);
    series1->attachAxis(axisY);
    series2->attachAxis(axisX);
    series2->attachAxis(axisY);
    series3->attachAxis(axisX);
    series3->attachAxis(axisY);

    zeroAxis();

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chart->setTitle(QString("Графики функции f(x) | a=%1, b=%2, c=%3, d=%4, e=%5")
                        .arg(currentA).arg(currentB).arg(currentC).arg(currentD).arg(currentE));
}

void MainWindow::onParameterChangedFirst()
{
    currentA = ui->aSpinBox->value();
    calculateFirstBranch();
    updateTable();
    updateGraph();
    sendParametersToServer();
}

void MainWindow::onParameterChangedSecond()
{
    currentB = ui->bSpinBox->value();
    calculateSecondBranch();
    updateTable();
    updateGraph();
    sendParametersToServer();
}

void MainWindow::onParameterChangedThird()
{
    currentC = ui->cSpinBox->value();
    currentD = ui->dSpinBox->value();
    currentE = ui->eSpinBox->value();
    calculateThirdBranch();
    updateTable();
    updateGraph();
    sendParametersToServer();
}

void MainWindow::onCalculateSingle()
{
    if (ui->xEdit->text().isEmpty()) {
        showError("Ошибка", "Введите значение x!");
        ui->xEdit->setFocus();
        return;
    }

    bool ok;
    double x = ui->xEdit->text().toDouble(&ok);

    if (!ok) {
        showError("Ошибка", "Введите корректное число!");
        ui->xEdit->setFocus();
        ui->xEdit->selectAll();
        return;
    }

    double result = f29(x, currentA, currentB, currentC, currentD, currentE);

    QString branch;
    if (x >= 0 && x < 1) {
        branch = "a·√x";
    } else if (x >= 1 && x < 2) {
        branch = "b/x";
    } else if (x >= 2) {
        branch = "c·x² - d·x + e";
    } else {
        branch = "не определена (x < 0)";
        result = 0;
    }

    ui->resultLabel->setText(QString("📐 Результат: f(%1) = %2   [ветка: %3]")
                                 .arg(x, 0, 'g', 4)
                                 .arg(result, 0, 'g', 10)
                                 .arg(branch));

    sendCalculationToServer(x, result);
}

void MainWindow::onBackToSolveClicked()
{
    emit backToSolveForm();
    this->close();
}

void MainWindow::onBackClicked()
{
    this->close();
}

void MainWindow::sendParametersToServer()
{
    if (TcpClient::getInstance().isConnected()) {
        QJsonObject data;
        data["a"] = currentA;
        data["b"] = currentB;
        data["c"] = currentC;
        data["d"] = currentD;
        data["e"] = currentE;
        TcpClient::getInstance().sendRequest("/param_change", data);
        qDebug() << "Параметры отправлены на сервер:" << currentA << currentB << currentC << currentD << currentE;
    }
}

void MainWindow::sendCalculationToServer(double x, double result)
{
    if (TcpClient::getInstance().isConnected()) {
        QJsonObject data;
        data["x"] = x;
        data["a"] = currentA;
        data["b"] = currentB;
        data["c"] = currentC;
        data["d"] = currentD;
        data["e"] = currentE;
        data["result"] = result;
        TcpClient::getInstance().sendRequest("/calculate", data);
        qDebug() << "Вычисление отправлено на сервер: x=" << x << "result=" << result;
    }
}
