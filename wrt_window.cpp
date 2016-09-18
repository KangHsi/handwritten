#include "wrt_window.h"
#include "ui_wrt_window.h"
#include "toolbox.h"
#include "iconhelper.h"

#include "globalDefs.h"
#include <QDebug>

//using namespace std;
wrt_window::wrt_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wrt_window)
  , mpLayout(NULL)
      , mpCanvas(NULL)
      , mpTabletInfos(NULL)
      , mpPenPressureItem(NULL)
      , mpPenXPos(NULL)
      , mpPenYPos(NULL)
      , mpPenXTilt(NULL)
      , mpPenYTilt(NULL)

{

   ui->setupUi(this);

    //
    mpLayout = new QHBoxLayout(ui->canvas);//announce a layout in canvas area
   // setLayout(verticalLayout);

    mpCanvas = new TabletCanvas(this);//a instance of canvas
    mpLayout->addWidget(mpCanvas,1);



    mpTabletInfos = new ToolPalette(this);
    ui->verticalLayout->addWidget(mpTabletInfos);
   // mpLayout->addWidget(mpTabletInfos);

    connect(mpCanvas, SIGNAL(tabletEventProcessed()), this, SLOT(onTabletEventProcessed()));
    initTabletInfos();
    //


    toolbox::FormInCenter(this);

    this->InitStyle();


    //tcp
    p= new QTcpSocket();
    hp = new QHostAddress("192.168.253.1");
    //
    WRT=new mydraw(this);


}

wrt_window::~wrt_window()
{

    DELETEPTR(mpPenXTilt);
    DELETEPTR(mpPenYTilt);
    DELETEPTR(mpPenPressureItem);
    DELETEPTR(mpPenXPos);
    DELETEPTR(mpPenYPos);
    DELETEPTR(mpLayout);
    DELETEPTR(mpCanvas);
    DELETEPTR(mpTabletInfos);
    delete ui;

}

void wrt_window::InitStyle()
{
    //hidden the title of the windows
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //install listener for double click the title
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
}

bool wrt_window::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void wrt_window::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

//mouse press drag
//void wrt_window::mousePressEvent(QMouseEvent *e)
//{
//    if (e->button() == Qt::LeftButton) {
//        mousePressed = true;
//        mousePoint = e->globalPos() - this->pos();
//        e->accept();
//    }
//}

void wrt_window::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void wrt_window::on_btnMenu_Close_clicked()
{
    qApp->exit();
}

void wrt_window::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
    }
    max = !max;
}

void wrt_window::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}


void wrt_window::on_next_button_clicked()
{

    //QString s = QString::number(x, 10);      //  10jinzhi
    //ui->lineEdit->setText(s);

    //itV QVector<cv::Point2f>

    //WRT->getFeat_8d(itV);




}


void wrt_window::on_connect_button_clicked()
{
    //connect the server
    p->connectToHost(*hp,6000);
    connect(p,SIGNAL(connected()),this,SLOT(sett()));
}


void wrt_window::sett()
{
    //success info
    QMessageBox::about(this,"about","connect success");
    connect(p,SIGNAL(readyRead()),this,SLOT(getmsg()));

}

void wrt_window::getmsg()
{
    //accept the info
    QByteArray tmp = p->readAll();
    QString tmp2(tmp);
    ui->label->setText(tmp2);
}


void wrt_window::on_subbmit_button_clicked()
{
    //fasongxinxi
    QString tmp = ui->lineEdit->text();
     p->write(tmp.toLatin1());
}

//for canvas
void wrt_window::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev);
}

void wrt_window::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
}
void wrt_window::initTabletInfos()
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

void wrt_window::updateTabletInfos()
{
    mpPenPressureItem->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->currentPenPressure()));
    mpPenXPos->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penXPos()));
    mpPenYPos->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penYPos()));
    mpPenXTilt->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penXTilt()));
    mpPenYTilt->setData(1, Qt::DisplayRole, QString("%0").arg(mpCanvas->penYTilt()));
}

void wrt_window::onTabletEventProcessed()
{
    updateTabletInfos();
    update();
}


//

