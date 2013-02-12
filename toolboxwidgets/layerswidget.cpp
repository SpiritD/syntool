#include "layerswidget.h"

LayersWidget::LayersWidget(QWidget *parent) :
    QWidget(parent)
{
    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0,2,0,0);
    vLayout->setSpacing(3);
    vLayout->setAlignment(Qt::AlignTop);

    productsLayout = new QVBoxLayout(this);
    productsLayout->setContentsMargins(0,0,0,0);
    productsLayout->setAlignment(Qt::AlignTop);

    otherLayout = new QVBoxLayout(this);
    otherLayout->setContentsMargins(0,0,0,0);
    otherLayout->setAlignment(Qt::AlignTop);

    setObjectName("LayersWidget");
    setStyleSheet(QString("QWidget#LayersWidget {background-color: "
                  "rgb(%1, %2, %3);}")
                  .arg(parent->palette().background().color().red())
                  .arg(parent->palette().background().color().green())
                  .arg(parent->palette().background().color().blue()));

    productLayersLbl = new QLabel(tr("Products:"), this);
    vLayout->addWidget(productLayersLbl);
    vLayout->addLayout(productsLayout);

    QFrame* hLine = new QFrame(this);
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);
    vLayout->addWidget(hLine);

    othersLayersLbl = new QLabel(tr("Others:"), this);
    vLayout->addWidget(othersLayersLbl);
    vLayout->addLayout(otherLayout);
}

void LayersWidget::addProduct(QString ProductId)
{
    LayerBoxWidget* layerBox = new LayerBoxWidget(ProductId, this);

    connect(layerBox, &LayerBoxWidget::removeLayer, this, &LayersWidget::removeLayer);
    connect(layerBox, &LayerBoxWidget::showLayer, this, &LayersWidget::showLayer);
    connect(layerBox, &LayerBoxWidget::changedTransparency, this, &LayersWidget::changedTransparency);

    productsLayout->addWidget(layerBox);
}
