#ifndef FORMTASKCHOICE_H
#define FORMTASKCHOICE_H

#include <QWidget>

namespace Ui {
class FormTaskChoice;
}

class FormTaskChoice : public QWidget
{
    Q_OBJECT

public:
    explicit FormTaskChoice(QWidget *parent = nullptr);
    ~FormTaskChoice();

signals:
    void taskSelected(int taskId);   // Сигнал: выбран вариант (29)

private slots:
    void onTask29Clicked();          // Обработчик кнопки "Вариант 29"

private:
    Ui::FormTaskChoice *ui;
};

#endif // FORMTASKCHOICE_H
