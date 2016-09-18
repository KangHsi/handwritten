#ifndef MYDRAW_H
#define MYDRAW_H
#include <opencv2\opencv.hpp>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv\cxcore.h"
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QVector>
#include <valarray>
#define  nlsize 64
#define typenum 3811
#define featdim 160
//using namespace cv;
class mydraw : public QObject
{
    Q_OBJECT
private:
    CvMat *mulW;
    CvMat *traindata ;
    CvMat *trainclass;
    CvMat *testdata;
    wchar_t wcs[3811];
    //pr_xu mypr;
public:
    explicit mydraw(QObject *parent = 0);
    ~mydraw();
    bool LoadPrototype(const char * txtname, const char* weihttxt);//get prototype to train data and class label

    QVector<double>  Cal8dFeature(const QVector<cv::Point2d> &re_im,int im_nlsize[]);
    cv::Mat MyPaint(const QVector<cv::Point2d> &imtp,const QVector<cv::Point2d> &re_im);
    QVector<cv::Point2d>  mydraw::bresenhan(double x1,double y1,double x2,double y2);
    QVector<double>  mydraw::getdataVec(QVector<cv::Point2f> re_imd);
    QVector<cv::Point2d>  mydraw::NSN_norm(const QVector<cv::Point2d> &re_im,int im_nlsize[]);
    double mydraw::fndmax(const QVector<cv::Point2d> &arr,int i);
    double mydraw::fndmin(const QVector<cv::Point2d> &arr,int i);
    std::valarray<int64> mydraw::sum_norm(const cv::Mat &m,int i);
    QVector<cv::Point2d>  mydraw::im_smooth(const QVector<cv::Point2d> &re_im);
    QVector<cv::Point2d>  mydraw::bresenhan_eqrespl(const QVector<cv::Point2d> &re_im);
    //
    bool mydraw::getFeat_8d(QVector<cv::Point2f> itV);
    bool matchFeat(int K,QVector<int> &res);
  //  void Extract8dFeature(IplImage *imgnl,QVector<Point2f>afNL,CvMat * featureVec,int row_num);
protected:
    void recognition(QString,QString);
    void initial_PR(const char * file);
signals:

public slots:
};

#endif // MYDRAW_H
