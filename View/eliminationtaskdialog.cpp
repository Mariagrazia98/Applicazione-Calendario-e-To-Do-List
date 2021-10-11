//
// Created by mary_ on 11/10/2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_EliminationTaskDialog.h" resolved

#include "eliminationtaskdialog.h"

EliminationTaskDialog::EliminationTaskDialog(QWidget *parent) :
        QDialog(parent),
        vBoxLayout(new QVBoxLayout),
        allRepetition(new QRadioButton("Delete all recurrences")),
        onlyThisRepetition(new QRadioButton("Delete only this recurrence")),
        dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close)) {

    vBoxLayout->addWidget(allRepetition);
    vBoxLayout->addWidget(onlyThisRepetition);
    vBoxLayout->addWidget(dialogButtonBox);
    this->setLayout(vBoxLayout);
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &EliminationTaskDialog::onAccept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void EliminationTaskDialog::onAccept(){
    if(allRepetition->isChecked()){
        emit(eliminateRecurrences(0));
    }
    else if(onlyThisRepetition->isChecked())
    {
        emit(eliminateRecurrences(1));
    }
    else{
        emit(eliminateRecurrences(-1));
    }
    close();
}
EliminationTaskDialog::~EliminationTaskDialog() {
}
