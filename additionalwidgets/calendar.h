#ifndef CALENDAR_H
#define CALENDAR_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QCalendarWidget>

class Calendar : public QWidget
{
    Q_OBJECT
public:
    explicit Calendar(QWidget *parent = 0);
    QCalendarWidget* calendar;
    QPushButton* okButton;

protected:
    QGridLayout* gridLayout;
    QPushButton* cancelButton;
signals:
    
public slots:
    
};

#endif // CALENDAR_H
