#ifndef PRODUCTSWIDGET_H
#define PRODUCTSWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>

#include <QCalendarWidget>
#include <QDate>
#include <QTime>
#include <QDateEdit>
#include <QTimeEdit>
#include "additionalwidgets/inputbox.h"
#include "additionalwidgets/productinfowidget.h"

#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDebug>
#include <QPushButton>
#include <QtXml/QDomDocument>

#include <QHash>
#include "network/downloadimage.h"
#include <QFile>
#include <QDir>
#include <QSettings>

#include "more/ProductStructures.h"
#include "more/structure.h"

#include "timeline.h"
#include "network/getgranules.h"

class ProductsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProductsWidget(ConfigData *configData, QWidget *parent = 0);
    void setSelectedProducts(QHash<QString, selectedProduct>* selectedProductsValue,
                             QHash<QString, Granule>* granulesHashValue);
    void setObjectsPointer(TimeLine* timeLine);

    void updateButtons();
protected:
    QGridLayout*           gLayout;
    QDomDocument*          dom;
    ConfigData*            _configData;
    QString                serverName;
    QString                cacheDir;
    QUrl                   urlProducts;
    QUrl                   urlGranules;
    QNetworkAccessManager* networkManager;
    QByteArray             currentRequest;

    QLabel*      productsLbl;
    QPushButton* viewProductInfo;

    QComboBox*   comboProducts;
    QLabel*      productImageLbl;
    QPixmap*     productImagePixmap;
    QPushButton* reloadProductsButton;

    QPushButton* leftTopButton;

    InputBox* North;
    InputBox* South;
    InputBox* West;
    InputBox* East;

    QLabel*      parametersLbl;
    QComboBox*   comboParameters;
    QPushButton* addProductLabel;
    QPushButton* addProductToFavoritesButton;

    QHash<QString, int>      parametersList;
    // <productNaiadId, Product>
    QHash<QString, Product>* productsHash;
    // <productId, productNaiadId>
    QHash<qint32, QString>*  productsIdName;

    QHash<QString, selectedProduct>* selectedProducts;
    QHash<QString, Granule>*         granulesHash;

    TimeLine*    timeLinePointer;
    GetGranules* getGranulesPointer;
    void getGranulesForNewProduct();
signals:
    void productAdded(QString productNaiadId, qint32 productId, ProductType::Type type);
    void productDeleted(QString productId);
    void productsHashSignal(QHash<QString, Product>* productsHash, QHash<qint32, QString>* productsIdName);

    void setCursorModeSignal(CursorMode::Mode value);
    
public slots:
    void currentProductChanged(int index);
    void slotReadyReadProductList();
    void getErrorProductList(QNetworkReply::NetworkError);

    void getNewGranules(int scale);

    void reloadProductsList();
    void addProduct(ProductType::Type productType = ProductType::Product, QString productName = "");
    void addSavedProducts(bool favoritesOnly=false);
    void addProductToFavorites(bool value);
    void slotProductInfo();

    void removeProduct(QString productId);

    void areaCoordsSlot(GeoCoords pos1, GeoCoords pos2);
//    void rightBottomCoordsSlot(qreal lat, qreal lon);
    void setCheckedButton(bool value);
    void setCursorModeSlot(CursorMode::Mode value);
};

#endif // PRODUCTSWIDGET_H
