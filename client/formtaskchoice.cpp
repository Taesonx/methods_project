#include "formtaskchoice.h"
#include "ui_formtaskchoice.h"

FormTaskChoice::FormTaskChoice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTaskChoice)
{
    ui->setupUi(this);

    connect(ui->task29Button, &QPushButton::clicked,
            this, &FormTaskChoice::onTask29Clicked);
}

FormTaskChoice::~FormTaskChoice()
{
    delete ui;
}

void FormTaskChoice::onTask29Clicked()
{
    emit taskSelected(29);
}
