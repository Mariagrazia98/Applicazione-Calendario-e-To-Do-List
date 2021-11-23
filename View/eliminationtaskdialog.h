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

/**
 * @details An EliminationTaskDialog appears when a user clicks on the delete icon connected to
 * a CalendarTodo or a CalendarEvent, particularly, when this is characterized by a series of recurrences.
 * Its goal is to let the user choose if he/she wants to delete all the recurrences or only the one selected.
 */
class EliminationTaskDialog : public QDialog {
Q_OBJECT

public:
    /**
     * Constructor
     * @param parent the parent QWidget
     */
    explicit EliminationTaskDialog(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~EliminationTaskDialog() override;

public slots:

    /**
    * @brief 'OK' button pressed callback
    */
    void onAccept();

signals:

    /**
     * @param type is 0 if all recurrences have to be deleted, 1 if only this one
     */
    void eliminateRecurrences(int type);

private:

    /* UI */

    QVBoxLayout *vBoxLayout;
    QRadioButton *allRepetition;        // to delete all the repetitions
    QRadioButton *onlyThisRepetition;   // to delete only this repetition
    QDialogButtonBox* dialogButtonBox;  // contains 'OK' and 'Close' buttons
};

#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_ELIMINATIONTASKDIALOG_H