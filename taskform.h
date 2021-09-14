#ifndef TASKFORM_H
#define TASKFORM_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QGroupBox>

namespace Ui {
class TaskForm;
}

class TaskForm : public QWidget
{
    Q_OBJECT

public:
    explicit TaskForm(QWidget *parent = nullptr);
    ~TaskForm();

private:
    Ui::TaskForm *ui;
};

#endif // TASKFORM_H
