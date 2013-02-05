#include "layerboxwidget.h"

LayerBoxWidget::LayerBoxWidget(QString ProductName, QWidget *parent) :
    QWidget(parent)
{
    _productName = ProductName;

    gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    removeLayerButton = new QPushButton(QIcon(":/icons/delete.png"), "", this);
    removeLayerButton->setFixedSize(20, 20);
    removeLayerButton->setIconSize(QSize(12,12));
    removeLayerButton->setToolTip(tr("Remove layer"));
    connect(removeLayerButton, &QPushButton::clicked, this, &LayerBoxWidget::removeLayerSlot);

    showLayerCheckBox = new QCheckBox(this);
    showLayerCheckBox->setToolTip(tr("Show layer"));
    connect(showLayerCheckBox, &QCheckBox::stateChanged, this, &LayerBoxWidget::showChangedSlot);

    transparencySlider = new QSlider(Qt::Horizontal, this);
    transparencySlider->setMaximum(100);
    transparencySlider->setValue(100);
    transparencySlider->setToolTip(tr("Transparency layer"));
    connect(transparencySlider, &QSlider::valueChanged, this, &LayerBoxWidget::changedTransparencySlot);

    productNameLabel = new QLabel(ProductName, this);
    productNameLabel->setWordWrap(true);

    percentLabel = new QLabel("100%", this);
    percentLabel->setFixedWidth(percentLabel->sizeHint().width());

    gridLayout->addWidget(showLayerCheckBox, 0,0);
    gridLayout->addWidget(productNameLabel, 0, 1, 2, 2);
    gridLayout->addWidget(removeLayerButton, 0,3, Qt::AlignRight);

    gridLayout->addWidget(transparencySlider, 2,0, 1, 3);
    gridLayout->addWidget(percentLabel, 2,3);
}

void LayerBoxWidget::changedTransparencySlot(int value)
{
    percentLabel->setText(QString::number(value)+"%");
    emit changedTransparency(_productName, value);
}

void LayerBoxWidget::removeLayerSlot()
{
    qDebug() << _productName;
    emit removeLayer(_productName);
}

void LayerBoxWidget::showChangedSlot(int state)
{
    qDebug() << state;
    emit showLayer(_productName, state);
}
