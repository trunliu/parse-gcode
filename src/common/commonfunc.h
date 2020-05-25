#ifndef COMMONFUNC_H
#define COMMONFUNC_H
#include <QVector>
class QPointF;

//状态结构体：储存元素的状态信息
typedef struct
{
    bool isRelative=true;
    bool isMeter=true;
    int  isLeft=-1;
    bool isQuickLine=true;
    bool isFire=false;
    int  isAcw=-1;
}QStatus;

//指令的类型
enum CodeType{
    G99_CODE,RELATIVE_CODE,ABSOLUTE_CODE,INCH_CODE,METER_CODE,
    L_KERF_CODE,R_KERF_CODE,CANCEL_KERF_CODE,QUICK_LINE_CODE,
    LINE_CODE,CW_ARC_CODE,ACW_ARC_CODE,FIRE_ON_CODE,
    FIRE_OFF_CODE,FINISH_CODE,NO_USE_CODE,
};


//公共函数类
class commonFunc
{
public:
    commonFunc();
public:
    //根据字符串ch分段
    static QStringList splitBy(QString text,QString ch);
    //将坐标值沿x轴反转处理
    static void reversePointByX_Axis(QPointF& point);
    //将坐标值按scale比例系数进行放缩
    static void expandPointByScale(QPointF& point,double scale);
    //将坐标值按xoff,yoff进行偏移
    static void offsetPoint(QPointF& point,int xoff,int yoff);
    static void offsetPoint1(QPointF& point,int xoff,int yoff);
    //弧度转为角度
    static void radianToAngle(double&);
};

//自定义复数类
class ComplexNum
{
public:
    ComplexNum();
    ComplexNum(double a,double b);

public:
    //复数的四则运算
    ComplexNum operator +(const ComplexNum& num);
    ComplexNum operator -(const ComplexNum& num);
    ComplexNum operator *(const ComplexNum& num);
    ComplexNum operator /(const ComplexNum& num);

    //其他函数,设置和取模
    void setComplexNumValue(double a,double b);
    double getComplexNumMold();
    double A();
    double B();

private:
    double a,b;
};

#endif // COMMONFUNC_H
