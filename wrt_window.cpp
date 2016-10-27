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
    hp = new QHostAddress("124.16.71.176");

    net_man = new QNetworkAccessManager(this);
    connect(net_man,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish(QNetworkReply*)));
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


/***************************************************for windows style*********************************************************************/
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
    /*//////////////////////////////////////////*/



}
//监听Qevent
bool wrt_window::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
//    if (event->type() == QEvent::MouseButtonPress) {
//     this->on_btnMenu_drag(event->type());
//     return true;
//    }
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
//void wrt_window::on_btnMenu_drag(QMouseEvent *e)
//{
//    mousePressed = true;
//    mousePoint = e->globalPos() - this->pos();
//    e->accept();
//}

void wrt_window::mousePressEvent(QMouseEvent *e)
{

    if (e->button()==Qt::RightButton)//recognition
    {



    }
}


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


//listening keyboard
void wrt_window::keyPressEvent(QKeyEvent *k)
{
    if(k->key() == Qt::Key_C)
    {
        qDebug("clear the canvas");
        mpCanvas->clean_lines();
        qDebug("clear the textplain");
        ui->plainTextEdit->clear();
    }
    else if(k->key()==Qt::Key_Alt)
    {
        ui->plainTextEdit->clear();

        QString line_list=mpCanvas->get_lines();

        QUrl url("http://api.hanvon.com/rt/ws/v1/hand/line?key=60c2b9fb-1e63-402f-8828-e0927e19c7bc&code=d4b92957-78ed-4c52-a004-ac3928b054b5");
        qDebug()<<url;
        QJsonObject data;

        data.insert("uid","111.195.212.77");
        QString type="chns";
        data.insert("lang",type);
        data.insert("data",line_list+",-1,0,-1,-1");

        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/octet-stream"));


        net_man->post(request, QJsonDocument(data).toJson());


        qDebug("finished recognition");
    }

}


/*************************FOR RECOGNITION********************************************************************************/
void wrt_window::on_next_button_clicked()
{


     if (ui->progressBar->value()==100)
     {
         QMessageBox::about(this,"about","Finished,you have to upload your results.");
     }
     else
     {
         mpCanvas->clean_lines();
         WRT->next_word();
         ui->progressBar->setValue(ui->progressBar->value()+25);
     }
    //QString s = QString::number(x, 10);      //  10jinzhi
    //ui->lineEdit->setText(s);

    //hanwang recognition
   // QString key="60c2b9fb-1e63-402f-8828-e0927e19c7bc";


//    QVector<cv::Point2f> itV=mpCanvas->get_lines();
//    bool feat=WRT->getFeat_8d(itV);
//    QVector<int> res;
//    bool sd=WRT->matchFeat(5,res);

}

void wrt_window::replyFinish(QNetworkReply * reply)
{

    if(reply->error() != QNetworkReply::NoError)
     {
                qDebug() << "Error:" << reply->errorString();
                return;
     }
     QByteArray buf = reply->readAll();
     QStringList result=WRT->recognition(buf);
     qDebug()<<result;
     QList<QString>::Iterator it = result.begin(),itend = result.end();
     int i = 0;
     //show the result
     WRT->vertification(result[0]);
     for (;it != itend-1; it++,i++)
     {
         ui->plainTextEdit->insertPlainText( QString::number(i+1, 10));
         ui->plainTextEdit->insertPlainText(*it);
         ui->plainTextEdit->insertPlainText("  ");
     }

}

/*//////////////////////////////////for networking////////////////*************************************************************************/

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
    QString tmp = ui->plainTextEdit->toPlainText();
     p->write(tmp.toLatin1());
}

/**************************************************************************************************************************/

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



