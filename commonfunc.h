#ifndef COMMONFUNC_H
#define COMMONFUNC_H
#include <QVector>

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

class QPoint;
//公共函数类
class commonFunc
{
public:
    commonFunc();
public:
    //根据字符串ch分段
    static QStringList splitBy(QString text,QString ch);
    //将坐标值沿x轴反转处理
    static void reversePointByX_Axis(QPoint& point);
};

#endif // COMMONFUNC_H
