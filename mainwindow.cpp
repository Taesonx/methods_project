#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mathfunction.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QTableWidgetItem>
#include <algorithm>

// Не используем using namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentA = 1.0;
    currentB = 1.0;
    currentC = 1.0;
    currentD = 29.0;
    currentE = 29.0;

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
            this, &MainWindow::onParameterChanged);
    connect(ui->bSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChanged);
    connect(ui->cSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChanged);
    connect(ui->dSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChanged);
    connect(ui->eSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onParameterChanged);

    connect(ui->calculateSingleButton, &QPushButton::clicked,
            this, &MainWindow::onCalculateSingle);
    connect(ui->backButton, &QPushButton::clicked,
            this, &MainWindow::onBackClicked);

    setupGraph();
    calculateRange();
    updateTable();
    updateGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
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
}

void MainWindow::setupGraph()
{
    QLineSeries *series = new QLineSeries();
    series->setName("f(x)");

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("График функции f(x)");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("x");
    axisX->setLabelFormat("%.1f");
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("f(x)");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->graphView->setChart(chart);
    ui->graphView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::calculateRange()
{
    currentA = ui->aSpinBox->value();
    currentB = ui->bSpinBox->value();
    currentC = ui->cSpinBox->value();
    currentD = ui->dSpinBox->value();
    currentE = ui->eSpinBox->value();

    xValues.clear();
    yValues.clear();

    for (int i = 0; i <= 500; i++) {
        double x = i * 10.0 / 500.0;
        double y = MathFunction::calculate(x, currentA, currentB, currentC, currentD, currentE);
        xValues.append(x);
        yValues.append(y);
    }
}

void MainWindow::updateTable()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"x", "f(x)"});

    for (int i = 0; i < xValues.size(); i += 10) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(xValues[i], 'f', 3)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(yValues[i], 'f', 6)));
    }

    ui->tableWidget->resizeColumnToContents(0);
    ui->tableWidget->resizeColumnToContents(1);
}

void MainWindow::updateGraph()
{
    QChart *chart = ui->graphView->chart();

    QList<QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries *series : seriesList) {
        chart->removeSeries(series);
        delete series;
    }

    QLineSeries *series = new QLineSeries();
    series->setName(QString("f(x) (a=%1, b=%2, c=%3, d=%4, e=%5)")
                        .arg(currentA).arg(currentB).arg(currentC).arg(currentD).arg(currentE));

    for (int i = 0; i < xValues.size(); i++) {
        series->append(xValues[i], yValues[i]);
    }

    chart->addSeries(series);

    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis *axis : axes) {
        chart->removeAxis(axis);
        delete axis;
    }

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("x");
    axisX->setLabelFormat("%.1f");
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("f(x)");
    axisY->setLabelFormat("%.2f");

    double yMin = *std::min_element(yValues.constBegin(), yValues.constEnd());
    double yMax = *std::max_element(yValues.constBegin(), yValues.constEnd());
    double yMargin = (yMax - yMin) * 0.1;
    if (yMargin == 0) yMargin = 1;
    axisY->setRange(yMin - yMargin, yMax + yMargin);

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle(QString("График функции f(x) | a=%1, b=%2, c=%3, d=%4, e=%5")
                        .arg(currentA).arg(currentB).arg(currentC).arg(currentD).arg(currentE));
}

void MainWindow::onParameterChanged()
{
    calculateRange();
    updateTable();
    updateGraph();
}

void MainWindow::onCalculateSingle()
{
    bool ok;
    double x = ui->xEdit->text().toDouble(&ok);

    if (!ok || x < 0) {
        ui->resultLabel->setText("📐 Результат: Ошибка! x ≥ 0");
        return;
    }

    double result = MathFunction::calculate(x, currentA, currentB, currentC, currentD, currentE);
    ui->resultLabel->setText(QString("📐 Результат: f(%1) = %2")
                                 .arg(x, 0, 'g', 4)
                                 .arg(result, 0, 'g', 10));
}

void MainWindow::onBackClicked()
{
    this->close();
}
