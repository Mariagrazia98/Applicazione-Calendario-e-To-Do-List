//
// Created by mary_ on 11/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_ELIMINATIONTASKDIALOG_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_ELIMINATIONTASKDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

class EliminationTaskDialog : public QDialog {
Q_OBJECT

public:
    explicit EliminationTaskDialog(QWidget *parent = nullptr);

    ~EliminationTaskDialog() override;

public slots:
    void onAccept();

signals:
    void eliminateRecurrences(int type);

private:
    QVBoxLayout *vBoxLayout;
    QRadioButton *allRepetition;
    QRadioButton *onlyThisRepetition;
    QDialogButtonBox* dialogButtonBox;
};

#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_ELIMINATIONTASKDIALOG_H
