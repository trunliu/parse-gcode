#include "commonfunc.h"
#include <QDebug>
#include <QPoint>
const double PI=3.1415926;

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

void commonFunc::offsetPoint(QPoint& point,int xoff,int yoff){
    point.rx()+=xoff;
    point.ry()+=yoff;
}

void commonFunc::radianToAngle(double& num){
    num=num/PI*180;
}


ComplexNum::ComplexNum():a(0),b(0)
{

}

ComplexNum::ComplexNum(double a,double b){
    this->a=a;
    this->b=b;
}

//复数的四则运算
ComplexNum ComplexNum::operator +(const ComplexNum& num){
    return ComplexNum(this->a+num.a,this->b+num.b);
}

ComplexNum ComplexNum::operator -(const ComplexNum& num){
    return ComplexNum(this->a-num.a,this->b-num.b);
}

//复数的相乘(a+bi)(c+di)=(ac-bd)+(bc+ad)i两个复数的积仍然是一个复数
ComplexNum ComplexNum::operator *(const ComplexNum& num){
    return ComplexNum(this->a*num.a-this->b*num.b,this->b*num.a+this->a*num.b);
}

//复数的除法
ComplexNum ComplexNum::operator /(const ComplexNum& num){
    if (!num.a && !num.b){
        qDebug()<<"除数不能位0";
        return ComplexNum(a, b);
    }else{
        return ComplexNum((a*num.a + b*num.b) / (num.a*num.a + num.b*num.b),
                          (b*num.a - a*num.b) / (num.a*num.a + num.b*num.b));
    }
}

//其他函数,设置和取模
void ComplexNum::setComplexNumValue(double a,double b){
    this->a=a;
    this->b=b;
}

double ComplexNum::getComplexNumMold(){
    return sqrt(a*a+b*b);
}

double ComplexNum::A(){
    return this->a;
}
double ComplexNum::B(){
    return this->b;
}

