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
    void taskSelected(int taskId);

private slots:
    void onTask29Clicked();

private:
    Ui::FormTaskChoice *ui;
};

#endif // FORMTASKCHOICE_H
