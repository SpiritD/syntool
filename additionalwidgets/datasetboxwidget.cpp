#include "datasetboxwidget.h"

DatasetBoxWidget::DatasetBoxWidget(QString serverName, Granule granule, QWidget *parent):
    QWidget(parent)
{
    m_serverName = serverName;

    gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);

    showDatasetCheck = new QCheckBox(this);
    connect(showDatasetCheck, &QCheckBox::stateChanged, this, &DatasetBoxWidget::checkedSlot);

    granuleNameLabel = new QLabel(granule.granuleName, this);
    granuleNameLabel->setMinimumSize(1,1);
    granuleNameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    m_granuleId = granule.granuleId;
    m_productId = granule.productId;

    moreActions = new QPushButton(QIcon(":/icons/down.png"), "", this);
    moreActions->setIconSize(QSize(16, 16));
    moreActions->setFixedSize(24, 20);
    connect(moreActions, &QPushButton::clicked, this, &DatasetBoxWidget::showMoreButtons);

    gridLayout->setColumnStretch(0, 0);
    gridLayout->addWidget(showDatasetCheck, 0,0);
    gridLayout->addWidget(granuleNameLabel, 0, 1, 2, 3, Qt::AlignLeft);

    gridLayout->addWidget(moreActions, 0, 4, 1,1,Qt::AlignRight);

    hLine = new QFrame(this);
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(hLine, 4, 0, 1, 4);

    buttonsCreated = false;

    resize(sizeHint());

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void DatasetBoxWidget::setChecked(bool checked)
{
    showDatasetCheck->setChecked(checked);
}

void DatasetBoxWidget::checkedSlot(int state)
{
    bool checked = (state == Qt::Unchecked) ? false : true;
    emit changedDisplayGranule(checked, m_granuleId, m_productId);
}

void DatasetBoxWidget::closeGranuleId(qint32 granuleId)
{
    if (m_granuleId == granuleId)
    {
        hide();
        deleteLater();
    }
}

void DatasetBoxWidget::changedTransparencySlot(qint32 transparentValue)
{
    percentLabel->setText(QString::number(transparentValue)+"%");
    emit changedTransparency(m_granuleId, m_productId, transparentValue);
}

void DatasetBoxWidget::showMoreButtons()
{
    if (!buttonsCreated)
    {
        buttonsWidget = new QWidget(this);
        buttonsHLayout = new QGridLayout(buttonsWidget);
        buttonsHLayout->setContentsMargins(0,0,0,0);
        buttonsHLayout->setSpacing(1);

        imageButton = new QPushButton(tr("Image"), this);
        imageButton->setMaximumHeight(20);
        imageButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        connect(imageButton, &QPushButton::clicked, this, &DatasetBoxWidget::actionImageSlot);

        opendapButton = new QPushButton("OPeNDAP", this);
        opendapButton->setMaximumHeight(20);
        opendapButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        connect(opendapButton, &QPushButton::clicked, this, &DatasetBoxWidget::actionOpendapSlot);

        ftpButton = new QPushButton("FTP", this);
        ftpButton->setMaximumHeight(20);
        ftpButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        connect(ftpButton, &QPushButton::clicked, this, &DatasetBoxWidget::actionFtpSlot);

        kmlButton = new QPushButton("KML", this);
        kmlButton->setMaximumHeight(20);
        kmlButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        connect(kmlButton, &QPushButton::clicked, this, &DatasetBoxWidget::actionKmlSlot);

        propertiesButton = new QPushButton(tr("Info"), this);
        propertiesButton->setMaximumHeight(20);
        propertiesButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        connect(propertiesButton, &QPushButton::clicked, this, &DatasetBoxWidget::actionPropertiesSlot);

        buttonsHLayout->addWidget(imageButton, 0,0,1,6);
        buttonsHLayout->addWidget(opendapButton, 0,6,1,9);
        buttonsHLayout->addWidget(ftpButton, 1,0,1,5);
        buttonsHLayout->addWidget(kmlButton, 1,5,1,5);
        buttonsHLayout->addWidget(propertiesButton, 1,10,1,5);

        buttonsWidget->hide();
        buttonsWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

        gridLayout->addWidget(buttonsWidget, 3, 0, 1, 5);
        buttonsCreated = true;

        // add transparency Slider and label
        transparencySlider = new QSlider(Qt::Horizontal, this);
        transparencySlider->setMaximum(100);
        transparencySlider->setValue(100);
        transparencySlider->setToolTip(tr("Transparency layer"));

        percentLabel = new QLabel("100%", this);
        percentLabel->setFixedWidth(percentLabel->sizeHint().width());

        connect(transparencySlider, &QSlider::valueChanged, this, &DatasetBoxWidget::changedTransparencySlot);
        gridLayout->addWidget(transparencySlider, 2,0, 1, 4);
        gridLayout->addWidget(percentLabel, 2,4);

        transparencySlider->hide();
        percentLabel->hide();
    }

    buttonsWidget->setVisible(!buttonsWidget->isVisible());
    transparencySlider->setVisible(!transparencySlider->isVisible());
    percentLabel->setVisible(!percentLabel->isVisible());
}

void DatasetBoxWidget::actionImageSlot()
{
    granuleActions(m_serverName, QString::number(m_granuleId), "image");
}

void DatasetBoxWidget::actionOpendapSlot()
{
    granuleActions(m_serverName, QString::number(m_granuleId), "opendap");
}

void DatasetBoxWidget::actionFtpSlot()
{
    granuleActions(m_serverName, QString::number(m_granuleId), "ftp");
}

void DatasetBoxWidget::actionKmlSlot()
{
    granuleActions(m_serverName, QString::number(m_granuleId), "kml");
}

void DatasetBoxWidget::actionPropertiesSlot()
{
    emit granulePropertiesSignal(m_granuleId);
}

void DatasetBoxWidget::setBold(bool value)
{
    QFont lblFont = granuleNameLabel->font();
    lblFont.setBold(value);
    granuleNameLabel->setFont(lblFont);
}

void DatasetBoxWidget::mousePressEvent(QMouseEvent *e)
{
    emit selectDataset(m_granuleId);
    e->accept();
}
