#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QTreeWidgetItem>

#include "TabletCanvas.h"
#include "toolpalette.h"

#define W_WIDTH         640
#define W_HEIGHT        480
#define W_TITLE         "Wacom Tablet Test"

class TestWidget : public QWidget
{
    Q_OBJECT

public:
    TestWidget(QWidget *parent = 0);
    ~TestWidget();

protected:
    void resizeEvent(QResizeEvent *ev);
    void paintEvent(QPaintEvent *ev);

private slots:
    void onTabletEventProcessed();

private:
    void initTabletInfos();
    void updateTabletInfos();

    QHBoxLayout* mpLayout;
    TabletCanvas* mpCanvas;
    ToolPalette* mpTabletInfos;

    QTreeWidgetItem* mpPenPressureItem;
    QTreeWidgetItem* mpPenXPos;
    QTreeWidgetItem* mpPenYPos;
    QTreeWidgetItem* mpPenXTilt;
    QTreeWidgetItem* mpPenYTilt;


    QWidget* w;
};

#endif // TESTWIDGET_H
