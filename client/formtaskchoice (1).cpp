#include "formtaskchoice.h"
#include "ui_formtaskchoice.h"


//КОНСТРУКТОР
FormTaskChoice::FormTaskChoice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTaskChoice)
{
    ui->setupUi(this);   // Загружаем UI

    // Подключаем кнопку "Вариант 29" к слоту
    connect(ui->task29Button, &QPushButton::clicked,
            this, &FormTaskChoice::onTask29Clicked);
}


//ДЕСТРУКТОР
FormTaskChoice::~FormTaskChoice()
{
    delete ui;
}


//ОБРАБОТЧИК КНОПКИ "ВАРИАНТ 29"
void FormTaskChoice::onTask29Clicked()
{
    emit taskSelected(29);   // Отправляем сигнал, что выбран вариант 29
}
