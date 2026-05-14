#include "formsolve.h"
#include "ui_formsolve.h"


//КОНСТРУКТОР ФОРМЫ
FormSolve::FormSolve(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSolve)
{
    ui->setupUi(this);   // Загружаем UI с функцией и аналитическими свойствами

    // Подключаем кнопки
    connect(ui->calculateButton, &QPushButton::clicked,
            this, &FormSolve::onCalculateButtonClicked);
    connect(ui->backButton, &QPushButton::clicked,
            this, &FormSolve::onBackButtonClicked);
}


//ДЕСТРУКТОР
FormSolve::~FormSolve()
{
    delete ui;
}


//ОБРАБОТЧИК КНОПКИ "ПОСТРОИТЬ ГРАФИК И ТАБЛИЦУ"
void FormSolve::onCalculateButtonClicked()
{
    // Значения параметров по умолчанию (a,b,c,d,e = 0)
    // В реальном приложении здесь можно было бы получать значения из полей ввода
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double d = 0.0;
    double e = 0.0;

    // Отправляем сигнал контроллеру, чтобы он открыл MainWindow с графиком
    emit calculateRequested(a, b, c, d, e);
}

//ОБРАБОТЧИК КНОПКИ "НАЗАД"
void FormSolve::onBackButtonClicked()
{
    emit backRequested();   // Сообщаем контроллеру: вернуться к выбору задачи
}
