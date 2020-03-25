#include "commonfunc.h"
#include <QDebug>
#include <QPoint>
commonFunc::commonFunc()
{

}


QStringList commonFunc::splitBy(QString text,QString ch){
    QStringList res;
    res=text.split(ch);
    return res;
}

//QPoint类整形精度的点，ry()来直接改变当前点的坐标并返回其引用
void commonFunc::reversePointByX_Axis(QPoint &point){
    point.ry()=-point.y();
}


void commonFunc::expandPointByScale(QPoint& point,double scale){
    point.rx()=point.x()*scale;
    point.ry()=point.y()*scale;
}
