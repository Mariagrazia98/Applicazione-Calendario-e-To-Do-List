//
// Created by mary_ on 11/10/2021.
//

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
    // 'OK' button pressed
    if(allRepetition->isChecked()){
        // user wants to delete all the repetitions
        emit(eliminateRecurrences(0));
    }
    else if(onlyThisRepetition->isChecked())
    {   //user wants to delete only this repetition
        emit(eliminateRecurrences(1));
    }
    else{
        // nothing happens, equivalent to 'Close' button
        emit(eliminateRecurrences(-1));
    }
    close();
}
EliminationTaskDialog::~EliminationTaskDialog() {
}
