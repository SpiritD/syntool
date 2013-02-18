#ifndef DATASETBOXWIDGET_H
#define DATASETBOXWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

#include <QDebug>

#include <more/ProductStructures.h>
#include <more/granuleactions.h>
#include <additionalwidgets/granuleinfowidget.h>

class DatasetBoxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetBoxWidget(QString serverName, Granule granule, QWidget *parent = 0);


protected:
    QGridLayout* gridLayout;
    QCheckBox* showDatasetCheck;
    QLabel* granuleNameLabel;
    QSlider* transparencySlider;
    QLabel* percentLabel;
    QFrame* hLine;

    QPushButton* moreActions;

    QGridLayout* buttonsHLayout;
    QWidget* buttonsWidget;

    QPushButton* imageButton;
    QPushButton* opendapButton;
    QPushButton* ftpButton;
    QPushButton* kmlButton;
    QPushButton* propertiesButton;
    bool buttonsCreated;

    qint32 _granuleId;
    QString _serverName;

signals:
    void changedTransparency(qint32 _granuleId, qint32 transparentValue);
    void granulePropertiesSignal(qint32 _granuleId);

public slots:
    void closeGranuleId(qint32 granuleId);
    void changedTransparencySlot(qint32 transparentValue);
    void showMoreButtons();

    void setChecked(bool checked);

    void actionImageSlot();
    void actionOpendapSlot();
    void actionFtpSlot();
    void actionKmlSlot();
    void actionPropertiesSlot();
};

#endif // DATASETBOXWIDGET_H