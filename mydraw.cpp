#include "mydraw.h"
#include<fstream> //for load file
#include<vector>

//#include "wintab.h"
//#include "pktdef.h"
using namespace cv;
using namespace std;
mydraw::mydraw(QObject *parent) : QObject(parent)
{
    initial_PR("3811hanzi.txt");



    //construct
    traindata = cvCreateMat(typenum,featdim,CV_32FC1);
    trainclass= cvCreateMat(typenum,1,CV_32FC1);
    testdata= cvCreateMat(1,featdim,CV_32FC1);
    mulW=cvCreateMat(512,512,CV_32FC1);

    //loadprototype

    char* type_txt= "xu_real_mean_intra_class.txt" ;
    char* weight_txt  = "xu_real_vec.txt";
    LoadPrototype(type_txt,weight_txt);

   //调用之后mulW的值huibianhua


}

mydraw::~mydraw(void)
{
    cvReleaseMat(&traindata);
    cvReleaseMat(&trainclass);
    cvReleaseMat(&testdata);
}
void mydraw::recognition(QString ans,QString input)
{

}

//把3k个类别read到wcs中
void mydraw::initial_PR(const char *file)
{
    std::wcin.imbue(std::locale("CHS"));
    std::wcout.imbue(std::locale("CHS"));

    std::wifstream fis(file);
    fis.imbue(std::locale("CHS"));

    wchar_t wcvec[10];
    int i=0;
    while (!fis.eof()){
        wchar_t wcx;
        do{
                wcx=fis.get();
        }while (wcx!=L'\t');
        do{
                wcx=fis.get();
        }while(wcx==L'\t');
        wcs[i]=wcx;
        fis.getline(wcvec,10);
        i++;
    }
    fis.close();
}

bool mydraw::LoadPrototype(const char * txtname, const char* weihttxt){
    //describe: 3811个类中心读入到traindata中
    //int typenum=3811;
    //int featdim=512;

    //read txt to memory and store traindata

    ifstream in(txtname);
    double * snglfeat = new double[512];
    for (int i=0; i<typenum; i++)
    {
        for (int j=0; j<featdim; j++){
            in>>snglfeat[j];
            //double t = snglfeat[j];
            cvSet2D(traindata,i,j,cvScalar((double)snglfeat[j]));
        }
    }
    in.close();

    //set train class label
    for (int i=0; i<typenum; i++)
    {
        //double t = (double)i;
        cvSet2D(trainclass,i,0,cvScalar((double)i));
    }

    //read weighing matrix to reduce dim
    ifstream in2(weihttxt);
    std::ofstream out("re_spW.txt");

    for (int i=0; i<512; i++)
    {
        for (int j=0; j<512; j++){ //160
            in2>>snglfeat[j];
            double t = snglfeat[j];
            cvSet2D(mulW,i,j,cvScalar((double)t));
            t = cvGet2D(mulW,i,j).val[0];
            out<< t <<"  ";
        }
        out<<endl;
    }
    in2.close();
    out.close();

    delete [] snglfeat;
    return true;
}


bool mydraw::getFeat_8d(QVector<Point2f> itV)
{

    CvMat * testfeat512= cvCreateMat(1,512,CV_32FC1);
    QVector<double>  fvec = getdataVec(itV);
    for(int i=0;i<512;i++){ //1*512 -> 1*160 wrong
        cvSet2D(testfeat512,0,i,cvScalar((double)fvec[i]));
    }
    CvMat * mulres3= cvCreateMat(1,512,CV_32FC1);
    cvGEMM(testfeat512,this->mulW,1,NULL,0,mulres3);

    for(int i=0;i<featdim;i++){ //1*512 -> 1*160 wrong
        double t3 = cvGet2D(mulres3,0,i).val[0];
        double t=t3;
        cvSet2D(testdata,0,i,cvScalar((double)t));
    }
    cvReleaseMat(&testfeat512);
    cvReleaseMat(&mulres3);
    return true;
}

QVector<double>  mydraw::getdataVec(QVector<Point2f> re_imd)
{

    int im_nlsize[2]={64,64};

        QVector<Point2d> re_im(re_imd.size());
        for (int i=0;i<re_imd.size();i++){
                float x_t=400.00*(re_imd[i].x+1.0);
                float y_t=300.00*(re_imd[i].y+1.0);
                re_im[i].x=(int)x_t;
                re_im[i].y=(int)y_t;
        }
        double x_min=fndmin(re_im,0)-1;
        double x_max=fndmax(re_im,0)+1;
        double y_min=fndmin(re_im,1)-1;
        double y_max=fndmax(re_im,1)+1;

        int dot_num=re_im.size();
        double delta_x=x_max-x_min;
        double delta_y=y_max-y_min;

        for(int i=0;i<dot_num;i++){
            re_im[i].x=floor((re_im[i].x-x_min)*(im_nlsize[0]-1)/delta_x+0.5)+1;
            re_im[i].y=floor((re_im[i].y-y_min)*(im_nlsize[1]-1)/delta_y+0.5)+1;
        }

        QVector<Point2d> re_spl=bresenhan_eqrespl(re_im);
        QVector<Point2d> re_norm=NSN_norm(re_spl,im_nlsize);

        QVector<Point2d> re_spl2=bresenhan_eqrespl(re_norm);


        QVector<Point2d> re_eqrespl;
        dot_num=re_spl2.size();
        int i=0;
        while(i<dot_num){
            re_eqrespl.push_back(re_spl2[i]);
            i=i+3;
        }

        QVector<Point2d> re_spl3=im_smooth(re_eqrespl);


        QVector<double> xnout = Cal8dFeature(re_spl3,im_nlsize);

        return xnout;
}





QVector<double>  mydraw::Cal8dFeature(const QVector<Point2d> &re_im,int im_nlsize[])
{

    int dot_num=re_im.size();

    double *a1=new double[dot_num-1];
    double *a2=new double[dot_num-1];

    int *d1=new int[dot_num-1];
    int *d2=new int[dot_num-1];

    for(int j=0;j<dot_num-1;j++){
        double dx=abs(re_im[j+1].x-re_im[j].x);
        double dy=abs(re_im[j+1].y-re_im[j].y);

        //求a1[j],a2[j]的值;
        if(dx==0&&dy==0){
            a1[j]=0;
            a2[j]=0;
        }
        else if(dx==dy){
            a1[j]=0;
            a2[j]=1;
        }
        else{
            double sqt=sqrt(dx*dx+dy*dy);
            a1[j]=abs(dx-dy)/sqt;
            a2[j]=dx<dy ? sqrt(2.0*dx/sqt):sqrt(2.0*dy/sqt);
        }

        //求d1[j]的值;
        if(dy<=dx){
            if(re_im[j].x<=re_im[j+1].x)
                d1[j]=7;
            else
                d1[j]=3;
        }
        else{
            if(re_im[j].y<=re_im[j+1].y)
                d1[j]=5;
            else
                d1[j]=1;
        }

        //求d2[j]的值;
        if(re_im[j].x<=re_im[j+1].x){
            if(re_im[j].y<=re_im[j+1].y)
                d2[j]=6;
            else
                d2[j]=8;
        }
        else{
            if(re_im[j].y<=re_im[j+1].y)
                d2[j]=4;
            else
                d2[j]=2;
        }
    }

    QVector<cv::Point2d> p[8];

    for(int j=0;j<dot_num-1;j++){
        //判断d1[]，向1，3，5，7中push数据
        p[d1[j]-1].push_back(cv::Point2d(j,a1[j]));
        //判断d2[]，向2，4，6，8中push数据
        p[d2[j]-1].push_back(cv::Point2d(j,a2[j]));
    }
    cv::Mat img_n[8];
    for(int j=0;j<8;j++){
        QVector<cv::Point2d> imtp=p[j];
        Mat img=MyPaint(imtp,re_im);
        img_n[j]=img*255;
        std::stringstream ss;
        ss<<(j+1);
        string name="paint_GB002"+ss.str()+".png";
        //imwrite(name,img_n[j]);
        Mat img_cp;
        cv::GaussianBlur(img,img_cp,cv::Size(9,9),5);
        img_n[j]=img_cp;
    }


    double sr_l=0;
    //double  *fvec=new double[512];
    QVector<double> fvec;
    //std::ofstream out("outvec.txt");
    int sr=0;
    for(int k=0;k<8;k++)
        for(int i=1;i<9;i++)
            for(int j=1;j<9;j++){
                Mat roi=img_n[k](cv::Range(i*8-8,i*8-1),cv::Range(j*8-8,j*8-1));
                sr_l=sum(roi)[0]/64.0;
                sr_l=sqrt(sr_l);
                //out<<sr_l<<" ";
                //sr_l++;
                //fvec[sr] = sr_l;
                fvec.push_back(sr_l);
                sr++;
            }
            //out<<endl;
    //out.close();

            delete [] a1;
            delete [] a2;
            delete [] d1;
            delete [] d2;
    return fvec;
}

cv::Mat mydraw::MyPaint(const QVector<Point2d> &imtp,const QVector<Point2d> &re_im)
{
    int im_nlsize[2]={64,64};
    Mat img(im_nlsize[0],im_nlsize[1],CV_32F,cv::Scalar(0));
    int s1=imtp.size();

    for(int j=0;j<s1;j++){
        double sr=imtp[j].x;			//序号;
        double vl=imtp[j].y;			//像素值;

        if(vl<=1.0&&vl>=0.0&&sr!=0){
            QVector<Point2d>	p=bresenhan(re_im[sr].x,re_im[sr].y,re_im[sr+1].x,re_im[sr+1].y);
            int s2=p.size();
            for(int i=0;i<s2;i++)
                img.at<float>(p[i].y-1,p[i].x-1)=vl;
                //img.at<>(p[i].y-1,p[i].x-1)=floor(vl+0.5);
        }
    }

    return img;
    }
//定义bresenhan函数;
QVector<Point2d>  mydraw::bresenhan(double x1,double y1,double x2,double y2)
{

    double x=x1,y=y1;
    double xd=abs(x2-x1),yd=abs(y2-y1);
    int dx=x2>x1 ? 1:-1;
    int dy=y2>y1 ? 1:-1;

    QVector<Point2d> p;
    if(xd>yd){
        double a=2*yd;
        double b=a-xd;
        double c=b-xd;

        while (1)
        {
            Point2d point(x,y);
            p.push_back(point);

            if (x==x2&&y==y2)
                break;
            if(b<0){
                b+=a;
                x+=dx;
            }
            else{
                b+=c;
                x+=dx;
                y+=dy;
            }
        }
    }
    else{
        double a=2*xd;
        double b=a-yd;
        double c=b-yd;

        while(1){
            Point2d point(x,y);
            p.push_back(point);

            if (x==x2&&y==y2)
                break;
            if(b<0){
                b+=a;
                y+=dy;
            }
            else{
                b+=c;
                x+=dx;
                y+=dy;
            }

        }
    }

    return p;
}

QVector<Point2d>  mydraw::NSN_norm(const QVector<Point2d> &re_im,int im_nlsize[])
{

    int64 alfa_H=2;
    int64 alfa_V=2;

    int dot_num=re_im.size();
    Mat ori_img(im_nlsize[0],im_nlsize[1],CV_8U,cv::Scalar(0));

    //由re_im记录的点坐标得到二值图
    for(int i=0;i<dot_num;i++)
        ori_img.at<uchar>(re_im[i].y-1,re_im[i].x-1)=1;

    //计算每列之和，得到行向量;

    valarray<int64> H=sum_norm(ori_img,0)+alfa_H;
    int64 sum_H=H.sum();

    //计算每行之和，得到列向量;
    valarray<int64> V=sum_norm(ori_img,1)+alfa_V;
    int64 sum_V=V.sum();

    //开始计算re_norm
    QVector<Point2d> re_norm;
    for(int i=0;i<dot_num;i++){
        double x=re_im[i].x;
        double y=re_im[i].y;
        Point2d p(0,0);

        //计算p.x
        for(int j=0;j<x;j++)
            p.x+=H[j];
        p.x=floor(p.x*im_nlsize[0]/(sum_H)+0.5);
        //计算p.y
        for(int j=0;j<y;j++)
            p.y+=V[j];
        p.y=floor(p.y*im_nlsize[1]/(sum_V)+0.5);

        re_norm.push_back(p);
    }
    return re_norm;
}


//找到x,y坐标的最小值;
double mydraw::fndmin(const QVector<Point2d> &arr,int i){
int k,n=arr.size();
double min_val;
if(i==0){
    min_val=arr[0].x;
    for(k=1;k<n-1;k++){
        min_val=min_val<arr[k].x ? min_val:arr[k].x;
    }
    return min_val;
}
else
{
    min_val=arr[0].y;
    for(k=1;k<n-1;k++){
        min_val=min_val<arr[k].y ? min_val:arr[k].y;
    }
    return min_val;
}
}

//找到x,y坐标的最大值;
double mydraw::fndmax(const QVector<Point2d> &arr,int i)
{
int k,n=arr.size();
double max_val;
if(i==0){
    max_val=arr[0].x;
    for(k=1;k<n;k++){
        max_val=max_val>arr[k].x ? max_val:arr[k].x;
    }
    return max_val;
}
else
{
    max_val=arr[0].y;
    for(k=1;k<n;k++){
        max_val=max_val>arr[k].y ? max_val:arr[k].y;
    }
    return max_val;
}
}

valarray<int64>  mydraw::sum_norm(const Mat &m,int i)
{
    int h=m.rows;
    int v=m.cols;
    if(i==0){
        valarray<int64> arr(v);
        for(int k=0;k<v;k++){
            Mat r=m.col(k);
            arr[k]=sum(r)[0];
        }
        return arr;
    }
    else{
        valarray<int64> arr(h);
        for(int k=0;k<h;k++){
            Mat c=m.row(k);
            arr[k]=sum(c)[0];
        }
        return arr;
    }
}



QVector<Point2d>  mydraw::im_smooth(const QVector<Point2d> &re_im)
{
    QVector<Point2d> re_smooth;
    const int dot_num=re_im.size();
    for(int i=0;i<dot_num;i++){
        if(i==0)
            re_smooth.push_back(re_im[i]);
        else if (i==dot_num-1)
            re_smooth.push_back(re_im[i]);
        else{
            Point2d p;
            p.x=floor((re_im[i-1].x+re_im[i].x+re_im[i+1].x)/3+0.5);
            p.y=floor((re_im[i-1].y+re_im[i].y+re_im[i+1].y)/3+0.5);
            re_smooth.push_back(p);
        }
    }
    return re_smooth;
}

QVector<Point2d>  mydraw::bresenhan_eqrespl(const QVector<Point2d> &re_im)
{

    int im_nlsize[2]={64,64};
    int dot_num=re_im.size();			//获得re_im的大小
    QVector<Point2d> re_spl;			//新建一个vector<Point2i>,作为返回值;

    for(int i=0;i<dot_num-1;i++){

        double x1=re_im[i].y;
        double y1=re_im[i].x;

        double x2=re_im[i+1].y;
        double y2=re_im[i+1].x;

        QVector<Point2d> p=bresenhan(x1,y1,x2,y2);				//调用bresenham算法,获得vector<Point2i> p;
        int p_num=p.size();

        for(int j=0;j<p_num;j++){
            if(p[j].x>=1&&p[j].x<=im_nlsize[0]&&p[j].y>=1&&p[j].y<im_nlsize[1]){
                double tmp=p[j].x;
                p[j].x=p[j].y;
                p[j].y=tmp;
                re_spl.push_back(p[j]);			//往re_spl添加Point2i值;
            }
        }

    }
    return re_spl;
}
