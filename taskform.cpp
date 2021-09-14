#include "taskform.h"
#include "ui_taskform.h"

TaskForm::TaskForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskForm)
{
    ui->setupUi(this);
    ui->beginDateTime->setDateTime(QDateTime::currentDateTime());
    ui->expireDateTime->setDateTime(QDateTime::currentDateTime());
}

TaskForm::~TaskForm()
{
    delete ui;
}
