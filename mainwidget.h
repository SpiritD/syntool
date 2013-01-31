#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include "windowwidget.h"
#include "topmenu.h"
#include "aboutwidget.h"
#include "timeline.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QToolBox>
#include <QSplitter>
#include <QMenuBar>
#include <QApplication>
#include <toolboxwidgets/layerswidget.h>
#include <toolboxwidgets/placewidget.h>
#include <toolboxwidgets/mapswidget.h>
#include <toolboxwidgets/productswidget.h>
#include <rightsidebar.h>
#include <network/getgranules.h>

#include <topmenuwidgets/settingswidget.h>
#include <QHash>

class TimeLine;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow(){}
    void setHostedWindow(QWindow *window);
    void keyPress(QKeyEvent* e);
//    void resizeEvent(QResizeEvent *e);

    QHash<QString, selectedProduct>* selectedProducts;
    QHash<QString, Granule>* granulesHash;

protected:
    QSplitter *splitter;
    WindowWidget* glwgt;
    TopMenu* topMenu;
    TimeLine* timeLine;
    RightSidebar* rightSidebar;

    PlaceWidget* PlaceWgt;
    MapsWidget* MapsWgt;
    ProductsWidget* ProductsWgt;
    LayersWidget* LayersWgt;

    QVBoxLayout* vlayout;
    QHBoxLayout* hlayout;
    QWidget* centralwgt;

    AboutWidget* aboutWgt;
    SettingsWidget* settingsWidget;
    GetGranules* getGranules;

    void createMenuBar();
    void createPythonConsole();
//    void keyPressEvent(QKeyEvent *e);

public slots:
    void aboutProgram();
    void showTimeLine();

    void showSettings();
};

#endif // MAINWIDGET_H
