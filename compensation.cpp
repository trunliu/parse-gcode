#include <QtWidgets>
#include "compensation.h"
#include "element.h"

const double PI=3.1415926;
int sgn(double d){
    return d>=0?1:-1;
}

Compensation::Compensation():
    elemVector(0),
    compensationElemVector(0),
    d(0)
{

}

//供外界输出数据
void Compensation::setVector(const QVector<Element*>& elemVector){
    this->elemVector=elemVector;
}

//开始刀补计算
QVector<Element*> Compensation::calulate(double d){
    compensationElemVector.clear();
    //双指针
    int i=0,j=0;
    this->d=d;
    //遍历elementVector，对开火和关火命令之间得图元进行刀补计算
    for(i=0;i<elemVector.size();++i){
        if(elemVector[i]->Sentence()=="M07"){
           for(j=i+1;j<elemVector.size();++j)
               if(elemVector[j]->Sentence()=="M08")
                   run(i+1,j-1);
           i=j+1;
        }   
    }  

    return compensationElemVector;
}

//刀补主函数，设置每两个element之间得转接类型、元素连接类型、时间类型
void Compensation::run(int beginIndex,int endIndex){
    for(int i=beginIndex;i<endIndex;++i){
        enum transType transtyp=transferType(elemVector[i],elemVector[i+1]);
        enum elemType elemtyp=elemsType(elemVector[i],elemVector[i+1]);
        enum timeType timeType=BUILDING;

        if     (i==beginIndex) timeType=BUILDING;
        else if(i==endIndex-1) timeType=CANCEL;
        else                   timeType=DOING;

        selectAlgorithm(elemVector[i],elemVector[i+1],transtyp,elemtyp,timeType);
    }
}

//判断转接类型
enum transType Compensation::transferType(Element* elem1,Element* elem2){
    double xl1,zl1,zl2,xl2;
    xl1=directionVector(elem1).x();
    zl1=directionVector(elem1).y();
    xl2=directionVector(elem2).x();
    zl2=directionVector(elem2).y();

    enum transType type;
    if     (sgn(d)*(xl2*zl1-xl1*zl2)>0)                       type=SHORTER;
    else if(sgn(d)*(xl2*zl1-xl1*zl2)<0 && xl1*xl2+zl1*zl2>=0) type=LONGER;
    else if(sgn(d)*(xl2*zl1-xl1*zl2)<0 && xl1*xl2+zl1*zl2<0)  type=INSERT;

    return type;
}

//计算方向向量
QPointF Compensation::directionVector(Element*elem){
    if(dynamic_cast<Shape*>(elem)->isArc()){
    //计算圆弧得方向向量

    }else{//计算直线得方向向量
        QPointF start=dynamic_cast<Shape*>(elem)->Start();
        QPointF end=dynamic_cast<Shape*>(elem)->End();
        double x0,z0,x1,z1;
        x0=start.x(),z0=start.y();
        x1=end.x(),z1=end.y();
        double d=sqrt((x1-x0)*(x1-x0)+(z1-z0)*(z1-z0));
        return QPointF((x1-x0)/d,(z1-z0)/d);
    }
}

//判断连接元素类型
enum elemType Compensation::elemsType(Element*element1,Element*element2){
    enum elemType type;
    Shape* elem1=dynamic_cast<Shape*>(element1);
    Shape* elem2=dynamic_cast<Shape*>(element2);
    if     (!elem1->isArc() && !elem2->isArc()) type=LINE_LINE;
    else if(!elem1->isArc() &&  elem2->isArc()) type=LINE_ARC;
    else if( elem1->isArc() && !elem2->isArc()) type=ARC_LINE;
    else if( elem1->isArc() &&  elem2->isArc()) type=ARC_ARC;
    return type;
}

//根据转接类型、元素连接类型、时间类型判断选用哪个公式进行补偿计算（未完成）
void Compensation::selectAlgorithm(Element*elem1,Element*elem2,transType trantyp,elemType elemtyp,timeType timetyp){
    switch (trantyp) {
    case SHORTER:
        switch (elemtyp) {
        case LINE_LINE:
            switch (timetyp) {
            case BUILDING:Short_LineLine_Building(elem1,elem2);break;
            case DOING:Short_LineLine_Doing(elem1,elem2);break;
            case CANCEL:Short_LineLine_End(elem1,elem2);break;
            default:break;
            }
            break;
        case LINE_ARC:break;
        case ARC_LINE:break;
        case ARC_ARC:break;
        default:break;
        }
        break;
    case LONGER:
        switch (elemtyp) {
        case LINE_LINE:
            switch (timetyp) {
            case BUILDING:Long_LineLine_Building(elem1,elem2);break;
            case DOING:Long_LineLine_Doing(elem1,elem2);break;
            case CANCEL:Long_LineLine_End(elem1,elem2);break;
            default:break;
            }
            break;
        case LINE_ARC:break;
        case ARC_LINE:break;
        case ARC_ARC:break;
        default:break;
        }
        break;
    case INSERT:
        break;
    default:
        break;
    }
}

//伸长型_线线_刀补建立
void Compensation::Long_LineLine_Building(Element*elem1,Element*elem2){
    qDebug()<<"------------------";
    qDebug()<<"伸长型_线线_刀补建立";

    //获取计算公式所需要的变量
    double xl1=directionVector(elem1).x();
    double zl1=directionVector(elem1).y();
    double xl2=directionVector(elem2).x();
    double zl2=directionVector(elem2).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs2=0,zs2=0;
    xs2=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;

    //设置新的线条起点和终点
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());   //刀补建立的起点与第一个元素的起点是重合的
    newElem1->setEnd(QPointF(xs2,zs2));

    //将刀补计算后新的线加入数组中
    compensationElemVector.push_back(newElem1);
}

//伸长型_线线_刀补撤销
void Compensation::Long_LineLine_End(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补撤销";
    qDebug()<<"------------------";

    //获取计算公式所需要的变量
    double xl1=directionVector(elem1).x();
    double zl1=directionVector(elem1).y();
    double xl2=directionVector(elem2).x();
    double zl2=directionVector(elem2).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0;
    xs1=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs1=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;

    //刀补撤退需要画最后的两条线
    //第一条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(dynamic_cast<Shape*>(elem2)->End());   //刀补撤退的终点与最后一条线的终点是重合

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
}

//伸长型_线线_刀补进行
void Compensation::Long_LineLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补进行";
    //获取计算公式所需要的变量
    double xl1=directionVector(elem1).x();
    double zl1=directionVector(elem1).y();
    double xl2=directionVector(elem2).x();
    double zl2=directionVector(elem2).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0;
    xs1=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs1=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;

    //刀补进行，画一条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    compensationElemVector.push_back(newElem1);
}

//缩短型_线线_刀补建立
void Compensation::Short_LineLine_Building(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_线线_刀补建立";
}

//缩短型_线线_刀补进行
void Compensation::Short_LineLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_线线_刀补进行";
}

//缩短型_线线_刀补撤销
void Compensation::Short_LineLine_End(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_线线_刀补撤销";
}
