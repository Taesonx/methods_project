#include "formsolve.h"
#include "ui_formsolve.h"

FormSolve::FormSolve(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSolve)
{
    ui->setupUi(this);

    connect(ui->calculateButton, &QPushButton::clicked,
            this, &FormSolve::onCalculateButtonClicked);
    connect(ui->backButton, &QPushButton::clicked,
            this, &FormSolve::onBackButtonClicked);
}

FormSolve::~FormSolve()
{
    delete ui;
}

void FormSolve::onCalculateButtonClicked()
{
    // Значения по умолчанию для варианта 29
    double a = 1.0;
    double b = 1.0;
    double c = 1.0;
    double d = 29.0;
    double e = 29.0;

    emit calculateRequested(a, b, c, d, e);
}

void FormSolve::onBackButtonClicked()
{
    emit backRequested();
}
