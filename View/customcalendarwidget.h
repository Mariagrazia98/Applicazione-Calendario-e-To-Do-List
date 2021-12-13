//
// Created by manuel on 09/10/2021.
//

#ifndef APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CUSTOMCALENDARWIDGET_H
#define APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include <QPainter>
#include <QList>
#include "../Model/calendarobject.h"

#include <algorithm>


/**
 * @brief Custom CalendarWidget class which overrides paintCell
 */
class CustomCalendarWidget : public QCalendarWidget {
Q_OBJECT
public:
    /* Constructor */

    /**
     *
     * @param parent the parent of this Widget
     */
    explicit CustomCalendarWidget(QWidget *parent = nullptr);

    /**
     * sets the list of CalendarObjets to check
     * @param calendarObjects CalendarObjet list to check to color days cells
     */
    void setCalendarObjects( QList<std::shared_ptr<CalendarObject>> calendarObjects);

protected:
    /**
     * custom paintCell function
     */
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

private:
    QList<std::shared_ptr<CalendarObject>> calendarObjects; // CalendarObjet list to check to color days cells

    /* Functions */

    /**
     * paints a rect accordingly to the Date position
     * @param painter the Painter object to paint
     * @param rect the QRect object to be drawn
     */
    static void paintDate(QPainter *painter, const QRect &rect) ;
};


#endif //APPLICAZIONE_CALENDARIO_E_TO_DO_LIST_ALL_IN_ONE_CUSTOMCALENDARWIDGET_H
