#include <QDebug>
#include "testwidget.h"
#include "globalDefs.h"

TestWidget::TestWidget(QWidget *parent)
    : QWidget(parent)
    , mpLayout(NULL)
    , mpCanvas(NULL)
    , mpTabletInfos(NULL)
    , mpPenPressureItem(NULL)
    , mpPenXPos(NULL)
    , mpPenYPos(NULL)
    , mpPenXTilt(NULL)
    , mpPenYTilt(NULL)
{
    setWindowTitle(W_TITLE);
    resize(W_WIDTH, W_HEIGHT);

    // Build the UI
    mpLayout = new QHBoxLayout();
    setLayout(mpLayout);

    mpCanvas = new TabletCanvas(this);
    mpLayout->addWidget(mpCanvas, 1);

    mpTabletInfos = new ToolPalette(this);
    mpLayout->addWidget(mpTabletInfos);

    connect(mpCanvas, SIGNAL(tabletEventProcessed()), this, SLOT(onTabletEventProcessed()));

    initTabletInfos();
}

TestWidget::~TestWidget()
{
    DELETEPTR(mpPenXTilt)
    DELETEPTR(mpPenYTilt)
    DELETEPTR(mpPenPressureItem)
    DELETEPTR(mpPenXPos)
    DELETEPTR(mpPenYPos)
    DELETEPTR(mpLayout)
    DELETEPTR(mpCanvas)
    DELETEPTR(mpTabletInfos)
}

void TestWidget::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev);
}

void TestWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
}

void TestWidget::initTabletInfos()
{
    QTreeWidgetItem* pRoot = mpTabletInfos->invisibleRootItem();
    mpPenPressureItem = new QTreeWidgetItem();
    mpPenPressureItem->setData(0, Qt::DisplayRole, "Pen pressure");
    mpPenXPos = new QTreeWidgetItem();
    mpPenXPos->setData(0, Qt::DisplayRole, "X position");
    mpPenYPos = new QTreeWidgetItem();
    mpPenYPos->setData(0, Qt::DisplayRole, "Y position");
    mpPenXTilt = new QTreeWidgetItem();
    mpPenXTilt->setData(0, Qt::DisplayRole, "X tilt");
    mpPenYTilt = new QTreeWidgetItem();
    mpPenYTilt->setData(0, Qt::DisplayRole, "Y tilt");

    pRoot->addChild(mpPenPressureItem);
    pRoot->addChild(mpPenXPos);
    pRoot->addChild(mpPenYPos);
    pRoot->addChild(mpPenXTilt);
    pRoot->addChild(mpPenYTilt);

    mpTabletInfos->resizeColumnToContents(0);
    updateTabletInfos();
}

void TestWidget::updateTabletInfos()
{
    mpPenPressureItem->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->currentPenPressure()));
    mpPenXPos->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penXPos()));
    mpPenYPos->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penYPos()));
    mpPenXTilt->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penXTilt()));
    mpPenYTilt->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penYTilt()));
}

void TestWidget::onTabletEventProcessed()
{
    updateTabletInfos();
    update();
}
