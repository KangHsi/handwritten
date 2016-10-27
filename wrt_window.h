#ifndef WRT_WINDOW_H
#define WRT_WINDOW_H

#include<QTcpServer>
#include<QTcpSocket>
#include<QHostAddress>
#include <QNetworkReply>
#include <QNetworkRequest>

#include<QMessageBox>
#include<QDialog>


#include <QResizeEvent>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QTreeWidgetItem>

#include "TabletCanvas.h"
#include "ToolPalette.h"
#include "mydraw.h"

namespace Ui {
class wrt_window;
}

class wrt_window : public QDialog
{
    Q_OBJECT

public:
    explicit wrt_window(QWidget *parent = 0);
    ~wrt_window();


protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
   // void on_btnMenu_drag(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *ev);
    void paintEvent(QPaintEvent *ev);
    void keyPressEvent(QKeyEvent *k);


private slots:
    void on_btnMenu_Close_clicked();

    void on_btnMenu_Max_clicked();

    void on_btnMenu_Min_clicked();

    void onTabletEventProcessed();

    void on_next_button_clicked();

    void on_connect_button_clicked();

    void on_subbmit_button_clicked();



public slots:
    void sett();
    void getmsg();
    void replyFinish(QNetworkReply*);

private:


    QTcpSocket *p;
    QHostAddress *hp;

    Ui::wrt_window *ui;
    mydraw* WRT;
    bool mousePressed;
    bool max;
    QRect location;
    QPoint mousePoint;
    QNetworkAccessManager* net_man;




    QHBoxLayout* mpLayout;
    QVBoxLayout* canvas_layout;
    TabletCanvas* mpCanvas;
    ToolPalette* mpTabletInfos;

    QTreeWidgetItem* mpPenPressureItem;
    QTreeWidgetItem* mpPenXPos;
    QTreeWidgetItem* mpPenYPos;
    QTreeWidgetItem* mpPenXTilt;
    QTreeWidgetItem* mpPenYTilt;

    //
    void initTabletInfos();
    void updateTabletInfos();
    void InitStyle();
    void updateServer(QString, int);

};

#endif // WRT_WINDOW_H
