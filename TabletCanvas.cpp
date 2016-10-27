#include "TabletCanvas.h"
#include <QDataStream>
#include <QFile>
#include <QPainter>
#include <QPen>
#include <qdebug.h>


TabletCanvas::TabletCanvas(QWidget *parent, const char *name):QWidget(parent)
  , mCrntPenPressure(0)
  , mPenXPos(0)
  , mPenYPos(0)
  , mPenXTilt(0)
  , mPenYTilt(0)
{
    setObjectName(name);
   // setStyleSheet(QString("background-color:red;"));

   // setStyleSheet(".QWidget{border-image: url(C:/Users/sk123/Documents/QTproject/WRT/image/check_back.png)}");
    this->isPressed = true;//FOR PENPRESSURE
    this->update();
    setCursor(Qt::CrossCursor);
}

TabletCanvas::~TabletCanvas()
{

}

//TABLET CONTROLLING
void TabletCanvas::tabletEvent(QTabletEvent *ev)
{

    switch(ev->type())
    {
        case QEvent::TabletPress:
        {

            mCrntPenPressure = ev->pressure();
            startPnt = ev->pos();
            endPnt = ev->pos();
            this->isPressed = true;

        }
            break;

        case QEvent::TabletMove:
        {
            mPenXPos = ev->x();
            mPenYPos = ev->y();
            mCrntPenPressure = ev->pressure();
            //draw
            endPnt = ev->pos();
             if (this->isPressed)//画笔抬起就不画了
             {
            myLine* line = new myLine;  //put the new line into vector
            line->startPnt = startPnt;
            line->endPnt = endPnt;
            this->lines.push_back(line);

            update();                                    //repainter，call paintEvent
            startPnt = endPnt;
             }
        }
        break;
       case QEvent::TabletRelease:
       {
           this->isPressed = false;
       }


        default:
            break;
    }

    mPenXTilt = ev->xTilt();
    mPenYTilt = ev->yTilt();

    emit tabletEventProcessed();
}

//every time repaint
void TabletCanvas::paintEvent(QPaintEvent* p)
{
    Q_UNUSED(p);

    QPainter painter(this);

    // set the color and width of the pen
     painter.setPen(QPen(QColor(0, 160, 230), 3));
    //QColor color(0, 0, 0, 50);
    //painter.setPen(color);
    //p.fillRect(this->rect(), QBrush(Qt::gray));


     //set the background
     QPalette pb;


     pb.setBrush(this->backgroundRole(),QBrush(QColor(66,66,66)));
     this->setPalette(pb);
     this->setAutoFillBackground(true);

     //drawing lines
     for(int i = 0;i<lines.size();i++){
             myLine* pLine = lines[i];
             painter.drawLine(pLine->startPnt,pLine->endPnt);
         }
}

QString TabletCanvas::get_lines()
{
    QString traj;

    int tmp=lines[0]->startPnt.x();
    QString str=QString::number(tmp, 10);
    traj.push_back(str+',');
    tmp=lines[0]->startPnt.y();
    str=QString::number(tmp, 10);
    traj.push_back(str+',');

//    tmp=lines[0]->endPnt.x();
//    str=QString::number(tmp, 10);
//    traj.push_back(str+',');
//    tmp=lines[0]->endPnt.y();
//    str=QString::number(tmp, 10);
//    traj.push_back(str+",-1,0,");

    qDebug()<<"lines size:"<<lines.size();
    for(int i = 1;i<lines.size()-1;i++)
    {

     int tmp_x=lines[i]->startPnt.x();
     int tmp_y=lines[i]->startPnt.y();
     if (tmp_x+tmp_y-lines[i-1]->startPnt.x()-lines[i-1]->startPnt.y()>1)
    {

          str=QString::number(tmp_x, 10);
          traj.push_back(str+',');
          str=QString::number(tmp_y, 10);
          traj.push_back(str+',');
//          tmp_x=lines[i]->endPnt.x();
//          tmp_y=lines[i]->endPnt.y();
//          str=QString::number(tmp_x, 10);
//          traj.push_back(str+',');
//          str=QString::number(tmp_y, 10);
//          traj.push_back(str+",-1,0,");

      }
    }

   return traj;

}

bool TabletCanvas::writeFile(const QString filePath)
{



    if(!filePath.isEmpty())
    {//判断路径是否为空
           QFile file(filePath); //定义指定路径文件
           if (!file.open(QIODevice::WriteOnly))
           {//只读方式打开failed

               //QMessageBox::about(this,"about","connect success");
               QMessageBox::warning(this, tr("写入文件"), tr("无法打开文件:\n%1").arg(filePath));
               return 0;
           }
           QDataStream out(&file); //4
           out.setVersion(QDataStream::Qt_4_9);
           out << QByteArray("ID:123");
           //写入点

           for(int m = 0;m<lines.size();m++)
           {

              out << (QPointF)lines[m]->startPnt;
              out << (QPointF)lines[m]->endPnt;


           }

           file.close();


//           if (!file.open(QIODevice::ReadOnly))
//              {
//                  //qDebug() << "Cannot open file for reading: ";
//                  return 0;
//              }
//              QDataStream in(&file);
//              in.setVersion(QDataStream::Qt_4_9);
//              int n;

//              QByteArray s;
//              in >> s;


//              for(int m = 0;m<lines.size();m++)
//              {
//                 QPoint Pnt;
//                 in >> Pnt;
//                 qDebug("stPnt:(%d,%d)",Pnt.x(),Pnt.y());

//                 in >> Pnt;

//              }

//              file.close();

      }
    return 0;
}

void TabletCanvas::clean_lines()
{
    lines.clear();
    qDebug("%d",lines.size());
    this->update();

}
