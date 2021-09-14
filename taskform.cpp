#include "taskform.h"
#include "ui_taskform.h"

#include <iostream>

TaskForm::TaskForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::TaskForm) {
    ui->setupUi(this);
    ui->beginDateTime->setDateTime(QDateTime::currentDateTime());
    ui->expireDateTime->setDateTime(QDateTime::currentDateTime());
}

TaskForm::~TaskForm() {
    delete ui;
}

void TaskForm::on_buttonBox_rejected() {
    this->close();
}

void TaskForm::on_comboBox_currentIndexChanged(int index) {
    switch (index) {
        case 0:
            ui->expireDateTime->setEnabled(true);
            ui->beginDateTime->setEnabled(true);
            break;
        case 1:
            // task
            ui->expireDateTime->setEnabled(false);
            ui->beginDateTime->setEnabled(false);
            break;
        default:
            break;
    }
}
