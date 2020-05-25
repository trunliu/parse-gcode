#include <QtWidgets>
#include "compensation.h"
#include "element.h"
#define TEST

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
           for(j=i+1;j<elemVector.size();++j){
               if(elemVector[j]->Sentence()=="M08"){
                   run(i+1,j-1);
                   break;
               }
           }
           i=j+1;
        }   
    }  

    return compensationElemVector;
}

//刀补主函数，设置每两个element之间得转接类型、元素连接类型、时间类型
void Compensation::run(int beginIndex,int endIndex){
#ifdef TEST
    qDebug()<<"========================开始刀补计算==========================";
    qDebug()<<"刀补计算第"<<beginIndex<<"行开始"<<"第"<<endIndex<<"行结束";
#endif
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

    xl1=dynamic_cast<Shape*>(elem1)->isArc() ? directionVector(elem1,2).x(): directionVector(elem1,0).x();
    zl1=dynamic_cast<Shape*>(elem1)->isArc() ? directionVector(elem1,2).y(): directionVector(elem1,0).y();
    xl2=dynamic_cast<Shape*>(elem2)->isArc() ? directionVector(elem2,1).x(): directionVector(elem2,0).x();
    zl2=dynamic_cast<Shape*>(elem2)->isArc() ? directionVector(elem2,1).y(): directionVector(elem2,0).y();

    enum transType type;
    if     (sgn(d)*(xl2*zl1-xl1*zl2)>=0)                      type=SHORTER;
    else if(sgn(d)*(xl2*zl1-xl1*zl2)<0 && xl1*xl2+zl1*zl2>=0) type=LONGER;
    else if(sgn(d)*(xl2*zl1-xl1*zl2)<0 && xl1*xl2+zl1*zl2<0)  type=INSERT;

    return type;
}

//计算方向向量
QPointF Compensation::directionVector(Element*elem,int pos){
    //计算圆弧起点的方向向量
    if(pos==1){
        //获取I,K
        QPointF centre=dynamic_cast<myArc*>(elem)->Centre();
        QPointF start=dynamic_cast<myArc*>(elem)->Start();
        double I=(centre-start).x();
        double K=(centre-start).y();
        //获取R
        double R=dynamic_cast<myArc*>(elem)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
        //圆弧起点得方向向量
        double xl=K/R;
        double zl=-I/R;
        return QPointF(xl,zl);
    //计算圆弧终点的方向向量
    }else if(pos==2){
        //获取I,K
        QPointF centre=dynamic_cast<myArc*>(elem)->Centre();
        QPointF end=dynamic_cast<myArc*>(elem)->End();
        double I=(centre-end).x();
        double K=(centre-end).y();
        //获取R
        double R=dynamic_cast<myArc*>(elem)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
        //圆弧起点得方向向量
        double xl=K/R;
        double zl=-I/R;
        return QPointF(xl,zl);
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
            case BUILDING:   Short_LineLine_Building(elem1,elem2);break;
            case DOING   :   Short_LineLine_Doing(elem1,elem2);break;
            case CANCEL  :   Short_LineLine_End(elem1,elem2);break;
            }
            break;
        case LINE_ARC:
            switch (timetyp) {
            case BUILDING:   Short_LineArc_Building(elem1,elem2);break;
            case DOING   :   Short_LineArc_Doing(elem1,elem2);break;
            case CANCEL  :   break;
            }
            break;
        case ARC_LINE:
            switch (timetyp) {
            case BUILDING:   break;
            case DOING   :   Short_ArcLine_Doing(elem1,elem2);break;
            case CANCEL  :   Short_ArcLine_End(elem1,elem2);break;
            }break;
        case ARC_ARC:
            switch (timetyp) {
            case BUILDING:   Short_ArcArc_Building(elem1,elem2);break;
            case DOING   :   Short_ArcArc_Doing(elem1,elem2);break;
            case CANCEL  :   Short_ArcArc_End(elem1,elem2);break;
            }break;
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
        case LINE_ARC:
            switch (timetyp) {
            case BUILDING:Long_LineArc_Building(elem1,elem2);break;
            case DOING:Long_LineArc_Doing(elem1,elem2);break;
            default:break;
            }break;
        case ARC_LINE:
            switch (timetyp) {
            case CANCEL:Long_ArcLine_End(elem1,elem2);break;
            case DOING:Long_ArcLine_Doing(elem1,elem2);break;
            default:break;
            }break;
        case ARC_ARC:
            switch (timetyp) {
            case DOING:Long_ArcArc_Doing(elem1,elem2);break;
            default:break;
            }break;
        default:break;
        }
        break;
    case INSERT:
        switch (elemtyp) {
        case LINE_LINE:
            switch (timetyp) {
            case BUILDING:Insert_LineLine_Building(elem1,elem2);break;
            case DOING:Insert_LineLine_Doing(elem1,elem2);break;
            case CANCEL:Insert_LineLine_End(elem1,elem2);break;
            default:break;
            }
            break;
        case LINE_ARC:
            switch (timetyp) {
            case BUILDING:Insert_LineArc_Building(elem1,elem2);break;
            case DOING:Insert_LineArc_Doing(elem1,elem2);break;
            default:break;
            }
            break;
        case ARC_LINE:
            switch (timetyp) {
            case DOING:Insert_ArcLine_Doing(elem1,elem2);break;
            case CANCEL:Insert_ArcLine_End(elem1,elem2);break;
            default:break;
            }break;
        case ARC_ARC:
            switch (timetyp) {
            case DOING:Insert_ArcArc_Doing(elem1,elem2);break;
            default:break;
            }
            break;
        default:break;
        }
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
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;

    //设置新的线条起点和终点
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());   //刀补建立的起点与第一个元素的起点是重合的
    newElem1->setEnd(QPointF(xs1,zs1));

    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    //将刀补计算后新的线加入数组中
    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
}

//伸长型_线线_刀补撤销
void Compensation::Long_LineLine_End(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补撤销";
    qDebug()<<"------------------";

    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0;
    xs1=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs1=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;
    xs2=x1+d*zl2;
    zs2=z1-d*xl2;


    //刀补撤退需要画最后的三条线
    //第一条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    //第三条线
    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(dynamic_cast<Shape*>(elem2)->End());   //刀补撤退的终点与最后一条线的终点是重合

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
}

//伸长型_线线_刀补进行
void Compensation::Long_LineLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补进行";
    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
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

    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0;
    xs1=x1+d*zl2;
    zs1=z1-d*xl2;

    //设置新的线条起点和终点
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());   //刀补建立的起点与第一个元素的起点是重合的
    newElem1->setEnd(QPointF(xs1,zs1));

    //将刀补计算后新的线加入数组中
    compensationElemVector.push_back(newElem1);
}

//缩短型_线线_刀补进行
void Compensation::Short_LineLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_线线_刀补进行";
    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0;
    if((zl1*xl2-zl2*xl1)==0)
    {
        xs1=x1+d*zl1;
        zs1=z1-d*xl1;
    }
    else
    {
        xs1=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
        zs1=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;
    }

    //刀补进行，画一条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    compensationElemVector.push_back(newElem1);
}

//缩短型_线线_刀补撤销
void Compensation::Short_LineLine_End(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_线线_刀补撤销";
    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

qDebug()<<zl1;
    //计算转接点
    double xs1=0,zs1=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;

    //刀补撤退需要画最后的两条线
    //第一条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));
 qDebug()<<dynamic_cast<Shape*>(compensationElemVector.back())->End();
 qDebug()<<QPointF(xs1,zs1);
    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(dynamic_cast<Shape*>(elem2)->End());   //刀补撤退的终点与最后一条线的终点是重合

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
}

//插入型_线线_刀补建立
void Compensation::Insert_LineLine_Building(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补建立";

    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=d*zl1+qAbs(d)*xl1+x1;
    zs2=-d*xl1+qAbs(d)*zl1+z1;
    xs3=d*zl2-qAbs(d)*xl2+x1;
    zs3=-d*xl2-qAbs(d)*zl2+z1;

    //设置新的线条起点和终点
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());   //刀补建立的起点与第一个元素的起点是重合的
    newElem1->setEnd(QPointF(xs1,zs1));

    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));

    //将刀补计算后新的线加入数组中
    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
}

//插入型_线线_刀补撤销
void Compensation::Insert_LineLine_End(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补撤销";
    qDebug()<<"------------------";

    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
    xs1=d*zl1+qAbs(d)*xl1+x1;
    zs1=-d*xl1+qAbs(d)*zl1+z1;
    xs2=d*zl2-qAbs(d)*xl2+x1;
    zs2=-d*xl2-qAbs(d)*zl2+z1;
    xs3=x1+d*zl2;
    zs3=z1-d*xl2;

    //刀补撤退需要画最后的四条线
    //第一条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    //第三条线
    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));

    //第四条线
    Line* newElem4=new Line;
    newElem4->setStart(QPointF(xs3,zs3));
    newElem4->setEnd(dynamic_cast<Shape*>(elem2)->End());

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
    compensationElemVector.push_back(newElem4);
}

//插入型_线线_刀补进行
void Compensation::Insert_LineLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线线_刀补进行";
    //获取计算公式所需要的变量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();
    double xl2=directionVector(elem2,0).x();
    double zl2=directionVector(elem2,0).y();
    double x1=dynamic_cast<Shape*>(elem1)->End().x();
    double z1=dynamic_cast<Shape*>(elem1)->End().y();

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0;
    xs1=d*xl1+qAbs(d)*xl1+x1;
    zs1=-d*xl1+qAbs(d)*zl1+z1;
    xs2=d*zl2-qAbs(d)*xl2+x1;
    zs2=-d*xl2-qAbs(d)*zl2+z1;

    //刀补进行，画两条线
    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
}

//缩短型_线弧_刀补建立
void Compensation::Short_LineArc_Building(Element*elem1,Element*elem2){

    qDebug()<<"缩短型_线弧_刀补建立";

    //获取计算公式所需要的变量
        //直线方向得向量
        double xl1=directionVector(elem1,0).x();
        double zl1=directionVector(elem1,0).y();

        //直线终点x1,z1
        double x1=dynamic_cast<Line*>(elem1)->End().x();
        double z1=dynamic_cast<Line*>(elem1)->End().y();

        //获取I,K
        QPointF centre=dynamic_cast<myArc*>(elem2)->Centre();
        QPointF start=dynamic_cast<myArc*>(elem2)->Start();
        double I=(centre-start).x();
        double K=(centre-start).y();

        //获取R
        double R=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);

        //圆弧起点得方向向量
        double xl2=K/R;
        double zl2=-I/R;

        //计算转接点
        double xs1=0,zs1=0;
            xs1=x1+d*zl2;
            zs1=z1-d*xl2;


            //设置新的线条起点和终点
            Line* newElem1=new Line;
            newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());  //刀补建立的起点与第一个元素的起点是重合的
            newElem1->setEnd(QPointF(xs1,zs1));


            //将刀补计算后新的线加入数组中
            compensationElemVector.push_back(newElem1);

}

//缩短型_线弧_刀补进行
void Compensation::Short_LineArc_Doing(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_线弧_刀补进行";
    //获取计算公式所需要的变量
    //直线方向得向量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();

    //直线终点x1,z1
    double x1=dynamic_cast<Line*>(elem1)->End().x();
    double z1=dynamic_cast<Line*>(elem1)->End().y();

    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem2)->Centre();
    QPointF start=dynamic_cast<myArc*>(elem2)->Start();
    double I=(centre-start).x();
    double K=(centre-start).y();

    //获取R
    double R=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);

    //圆弧起点得方向向量
    double xl2=K/R;
    double zl2=-I/R;

    //计算转接点
    double xs1=0,zs1=0;
    if((zl1*xl2-zl2*xl1)==0){
        xs1=x1+d*zl1;
        zs1=z1-d*xl1;
    }else{
        double f=sqrt((R+d)*(R+d)-(zl2*I-xl1*K-d)*(zl2*I-xl1*K-d));
        xs1=xl1*(xl1*I+zl1*K)+d*zl1-sgn(zl1*K+xl1*I)*xl1*f+x1;
        zs1=zl1*(xl1*I+zl1*K)-d*xl1-sgn(zl1*K+xl1*I)*zl1*f+z1;
    }

    Line* newElem1=new Line;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    compensationElemVector.push_back(newElem1);
}

//伸长型_线弧_刀补建立
void Compensation::Long_LineArc_Building(Element*elem1,Element*elem2){

    qDebug()<<"伸长型_线弧_刀补建立";

    //获取计算公式所需要的变量
        //直线方向得向量
        double xl1=directionVector(elem1,0).x();
        double zl1=directionVector(elem1,0).y();

        //直线终点x1,z1
        double x1=dynamic_cast<Line*>(elem1)->End().x();
        double z1=dynamic_cast<Line*>(elem1)->End().y();

        //获取I,K
        QPointF centre=dynamic_cast<myArc*>(elem2)->Centre();
        QPointF start=dynamic_cast<myArc*>(elem2)->Start();
        double I=(centre-start).x();
        double K=(centre-start).y();

        //获取R
        double R=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);

        //圆弧起点得方向向量
        double xl2=K/R;
        double zl2=-I/R;

        //计算转接点
        double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
            xs1=x1+d*zl1;
            zs1=z1-d*xl1;
            xs2=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
            zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;
            xs3=x1+d*zl2;
            zs3=z1-d*xl2;

            //设置新的线条起点和终点
            Line* newElem1=new Line;
            newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());  //刀补建立的起点与第一个元素的起点是重合的
            newElem1->setEnd(QPointF(xs1,zs1));

            Line* newElem2=new Line;
            newElem2->setStart(QPointF(xs1,zs1));
            newElem2->setEnd(QPointF(xs2,zs2));

            Line* newElem3=new Line;
            newElem3->setStart(QPointF(xs2,zs2));
            newElem3->setEnd(QPointF(xs3,zs3));

            //将刀补计算后新的线加入数组中
            compensationElemVector.push_back(newElem1);
            compensationElemVector.push_back(newElem2);
            compensationElemVector.push_back(newElem3);


}

//伸长型_线弧_刀补进行
void Compensation::Long_LineArc_Doing(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_线弧_刀补进行";
    //获取计算公式所需要的变量
    //直线方向得向量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();

    //直线终点x1,z1
    double x1=dynamic_cast<Line*>(elem1)->End().x();
    double z1=dynamic_cast<Line*>(elem1)->End().y();

    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem2)->Centre();
    QPointF start=dynamic_cast<myArc*>(elem2)->Start();
    double I=(centre-start).x();
    double K=(centre-start).y();

    //获取R
    double R=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);

    //圆弧起点得方向向量
    double xl2=K/R;
    double zl2=-I/R;

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0;

    xs1=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs1=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;

    xs2=x1+d*zl2;
    zs2=z1-d*xl2;

    Line* newElem1=new Line;
    //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End());
    newElem1->setEnd(QPointF(xs1,zs1));
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);

}

//插入型_线弧_刀补建立
void Compensation::Insert_LineArc_Building(Element*elem1,Element*elem2){

    qDebug()<<"插入型_线弧_刀补建立";

    //获取计算公式所需要的变量
        //直线方向得向量
        double xl1=directionVector(elem1,0).x();
        double zl1=directionVector(elem1,0).y();

        //直线终点x1,z1
        double x1=dynamic_cast<Line*>(elem1)->End().x();
        double z1=dynamic_cast<Line*>(elem1)->End().y();

        //获取I,K
        QPointF centre=dynamic_cast<myArc*>(elem2)->Centre();
        QPointF start=dynamic_cast<myArc*>(elem2)->Start();
        double I=(centre-start).x();
        double K=(centre-start).y();

        //获取R
        double R=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);

        //圆弧起点得方向向量
        double xl2=K/R;
        double zl2=-I/R;

        //计算转接点
        double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0,xs4=0,zs4=0;
            xs1=x1+d*zl1;
            zs1=z1-d*xl1;
            xs2=d*zl1+qAbs(d)*xl1+x1;
            zs2=-d*xl1+qAbs(d)*zl1+z1;
            xs3=d*zl2-qAbs(d)*xl2+x1;
            zs3=-d*xl2-qAbs(d)*zl2+z1;
            xs4=x1+d*zl2;
            zs4=z1-d*xl2;

            //设置新的线条起点和终点
            Line* newElem1=new Line;
            newElem1->setStart(dynamic_cast<Shape*>(elem1)->Start());  //刀补建立的起点与第一个元素的起点是重合的
            newElem1->setEnd(QPointF(xs1,zs1));

            Line* newElem2=new Line;
            newElem2->setStart(QPointF(xs1,zs1));
            newElem2->setEnd(QPointF(xs2,zs2));

            Line* newElem3=new Line;
            newElem3->setStart(QPointF(xs2,zs2));
            newElem3->setEnd(QPointF(xs3,zs3));

            Line* newElem4=new Line;
            newElem4->setStart(QPointF(xs3,zs3));
            newElem4->setEnd(QPointF(xs4,zs4));


            //将刀补计算后新的线加入数组中
            compensationElemVector.push_back(newElem1);
            compensationElemVector.push_back(newElem2);
            compensationElemVector.push_back(newElem3);
            compensationElemVector.push_back(newElem4);


}

//插入型_线弧_刀补进行
void Compensation::Insert_LineArc_Doing(Element*elem1,Element*elem2){

    qDebug()<<"插入型_线弧_刀补进行";
    //获取计算公式所需要的变量
    //直线方向得向量
    double xl1=directionVector(elem1,0).x();
    double zl1=directionVector(elem1,0).y();

    //直线终点x1,z1
    double x1=dynamic_cast<Line*>(elem1)->End().x();
    double z1=dynamic_cast<Line*>(elem1)->End().y();

    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem2)->Centre();
    QPointF start=dynamic_cast<myArc*>(elem2)->Start();
    double I=(centre-start).x();
    double K=(centre-start).y();

    //获取R
    double R=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);

    //圆弧起点得方向向量
    double xl2=K/R;
    double zl2=-I/R;

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
    xs1=d*zl1+qAbs(d)*xl1+x1;
    zs1=-d*xl1+qAbs(d)*zl1+z1;
    xs2=d*zl2-qAbs(d)*xl2+x1;
    zs2=-d*xl2-qAbs(d)*zl2+z1;
    xs3=x1+d*zl2;
    zs3=z1-d*xl2;


        Line* newElem1=new Line;
        newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
        newElem1->setEnd(QPointF(xs1,zs1));
        Line* newElem2=new Line;
        newElem2->setStart(QPointF(xs1,zs1));
        newElem2->setEnd(QPointF(xs2,zs2));
        Line* newElem3=new Line;
        newElem3->setStart(QPointF(xs2,zs2));
        newElem3->setEnd(QPointF(xs3,zs3));

        compensationElemVector.push_back(newElem1);
        compensationElemVector.push_back(newElem2);
        compensationElemVector.push_back(newElem3);

}


//缩短型_弧线_刀补进行
void Compensation::Short_ArcLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_弧线_刀补进行";
    //获取计算公式所需要的变量
    QPointF start=dynamic_cast<Line*>(elem2)->Start();
    QPointF end=dynamic_cast<Line*>(elem2)->End();
    double x2,z2,x1,z1;
    x1=start.x(),z1=start.y();
    x2=end.x(),z2=end.y();
    double D=sqrt((x2-x1)*(x2-x1)+(z2-z1)*(z2-z1));
    double xl2=(x2-x1)/D;
    double zl2=(z2-z1)/D;
    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF start2=dynamic_cast<myArc*>(elem1)->Start();
    double I=(centre-start2).x();
    double K=(centre-start2).y();

    //获取R
    double R=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
    double x0=start2.x(),z0=start2.y();
    double x01=x0+I-x1;
    double z01=z0+K-z1;
    double xl1=z01/R;
    double zl1=-x01/R;

    //计算转接点
    double xs1=0,zs1=0;
    if((zl1*xl2-zl2*xl1)==0){
        xs1=x1+d*zl2;
        zs1=z1-d*xl2;
    }else{
        double f=sqrt((R+d)*(R+d)-(zl2*x01-xl2*z01-d)*(zl2*x01-xl2*z01-d));
        xs1=xl2*(zl2*z01+xl2*x01)+d*zl2-sgn(zl2*z01+xl2*x01)*xl2*f+x1;
        zs1=zl2*(zl2*z01+xl2*x01)-d*zl2-sgn(zl2*z01+xl2*x01)*zl2*f+z1;
    }

    //刀补进行，画一条线
    //从上条线的终点画条弧到转接点
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));
    newElem1->setCentre(centre);

    compensationElemVector.push_back(newElem1);
}


void Compensation::Short_ArcLine_End(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_弧线_刀补撤销";
    //获取计算公式所需要的变量
    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF end=dynamic_cast<myArc*>(elem1)->End();
    double I=(centre-end).x();
    double K=(centre-end).y();

    //获取R 弧得终点方向向量
    double R=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
    double xl1=-K/R;
    double zl1=I/R;

    QPointF start=dynamic_cast<Line*>(elem2)->Start();
    double x1,z1;
    x1=start.x(),z1=start.y();

    //计算转接点
    double xs1=0,zs1=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;

    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End());
    newElem1->setEnd(QPointF(xs1,zs1));
    newElem1->setCentre(centre);

    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(dynamic_cast<Shape*>(elem2)->End());

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
}

//伸长型_弧线_刀补撤销
void Compensation::Long_ArcLine_End(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_弧线_刀补撤销";
    //获取计算公式所需要的变量
    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF end1=dynamic_cast<myArc*>(elem1)->End();
    double I=(centre-end1).x();
    double K=(centre-end1).y();
    //获取R 和弧得方向向量
    double R=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
    double xl1=K/R;
    double zl1=-I/R;

    //获取直线得方向向量
    double x2,z2,x1,z1;
    QPointF start=dynamic_cast<Line*>(elem2)->Start();
    QPointF end2=dynamic_cast<Line*>(elem2)->End();
    x1=start.x(),z1=start.y();
    x2=end2.x(),z2=end2.y();
    double D=sqrt((x2-x1)*(x2-x1)+(z2-z1)*(z2-z1));
    double xl2=(x2-x1)/D;
    double zl2=(z2-z1)/D;


    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=(xl1-xl2)*d/(zl2*xl1-zl1*xl2)+x1;
    zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;
    xs3=x1+d*zl2;
    zs3=z1-d*xl2;

    //第一条弧
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    //第三条线
    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));

    //第四条线
    Line* newElem4=new Line;
    newElem4->setStart(QPointF(xs3,zs3));
    newElem4->setEnd(dynamic_cast<Shape*>(elem2)->End());

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
    compensationElemVector.push_back(newElem4);


}

//伸长型_弧线_刀补进行
void Compensation::Long_ArcLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_弧线_刀补进行";
    //获取计算公式所需要的变量
    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF end1=dynamic_cast<myArc*>(elem1)->End();
    double I=(centre-end1).x();
    double K=(centre-end1).y();
    //获取R
    double R=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
    double xl1=K/R;
    double zl1=-I/R;

    double x2,z2,x1,z1;
    QPointF start=dynamic_cast<Line*>(elem2)->Start();
    QPointF end2=dynamic_cast<Line*>(elem2)->End();
    x1=start.x(),z1=start.y();
    x2=end2.x(),z2=end2.y();
    double D=sqrt((x2-x1)*(x2-x1)+(z2-z1)*(z2-z1));
    double xl2=(x2-x1)/D;
    double zl2=(z2-z1)/D;

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;

    //从上条线的终点画条弧到转接点
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));
    //设置圆心
    newElem1->setCentre(centre);

    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
}

//插入型_弧线_刀补撤销
void Compensation::Insert_ArcLine_End(Element*elem1,Element*elem2){

    qDebug()<<"插入型_弧线_刀补撤销";

    //获取计算公式所需要的变量
    QPointF start=dynamic_cast<Line*>(elem2)->Start();
    QPointF end=dynamic_cast<Line*>(elem2)->End();
    double x2,z2,x1,z1;
    x1=start.x(),z1=start.y();
    x2=end.x(),z2=end.y();
    double d=sqrt((x2-x1)*(x2-x1)+(z2-z1)*(z2-z1));
    double xl2=(x2-x1)/d;
    double zl2=(z2-z1)/d;
    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF start2=dynamic_cast<myArc*>(elem1)->Start();
    double I=(centre-start2).x();
    double K=(centre-start2).y();

    //获取R
    double R=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
    double x0=start2.x(),z0=start2.y();
    double x01=x0+I-x1;
    double z01=z0+K-z1;
    double xl1=-z01/R;
    double zl1=x01/R;

        //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0,xs4=0,zs4=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=d*zl1+qAbs(d)*xl1+x1;
    zs2=-d*xl1+qAbs(d)*zl1+z1;
    xs3=d*zl2-qAbs(d)*xl2+x1;
    zs3=-d*xl2-qAbs(d)*zl2+z1;
    xs4=x1+d*zl2;
    zs4=z1-d*xl2;

    //第一条弧
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    //第二条线
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    //第三条线
    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));

    //第四条线
    Line* newElem4=new Line;
    newElem4->setStart(QPointF(xs3,zs3));
    newElem4->setEnd(QPointF(xs4,zs4));

    //第五条线
    Line* newElem5=new Line;
    newElem4->setStart(QPointF(xs4,zs4));
    newElem4->setEnd(dynamic_cast<Shape*>(elem2)->End());

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
    compensationElemVector.push_back(newElem4);
    compensationElemVector.push_back(newElem5);

}

//插入型_弧线_刀补进行
void Compensation::Insert_ArcLine_Doing(Element*elem1,Element*elem2){
    qDebug()<<"插入型_弧线_刀补进行";
    //获取计算公式所需要的变量
    QPointF start=dynamic_cast<Line*>(elem2)->Start();
    QPointF end=dynamic_cast<Line*>(elem2)->End();
    double x2,z2,x1,z1;
    x1=start.x(),z1=start.y();
    x2=end.x(),z2=end.y();
    double d=sqrt((x2-x1)*(x2-x1)+(z2-z1)*(z2-z1));
    double xl2=(x2-x1)/d;
    double zl2=(z2-z1)/d;
    //获取I,K
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF start2=dynamic_cast<myArc*>(elem1)->Start();
    double I=(centre-start2).x();
    double K=(centre-start2).y();

    //获取R
    double R=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I*I+K*K) : -sqrtf(I*I+K*K);
    double x0=start2.x(),z0=start2.y();
    double x01=x0+I-x1;
    double z01=z0+K-z1;
    double xl1=-z01/R;
    double zl1=x01/R;

    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=d*zl1+qAbs(d)*xl1+x1;
    zs2=-d*xl1+qAbs(d)*zl1+z1;
    xs3=d*zl2-qAbs(d)*xl2+x1;
    zs3=-d*xl2-qAbs(d)*zl2+z1;


    //从上条线的终点画条弧到转接点
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));

    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
}

void Compensation::Short_ArcArc_Doing(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_弧弧_刀补进行";
    //获取计算公式所需要的变量
    QPointF start=dynamic_cast<myArc*>(elem1)->Start();
    QPointF end=dynamic_cast<myArc*>(elem1)->End();
    double x0=start.x(),z0=start.y();
    double x1=end.x(),z1=end.y();
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    double I1=(centre-start).x();
    double K1=(centre-start).y();
    QPointF centre2=dynamic_cast<myArc*>(elem2)->Centre();
    double I2=(centre2-end).x();
    double K2=(centre2-end).y();
    double x01=(x0+I1)-x1;
    double z01=(z0+K1)-z1;
    double x02=I2;
    double z02=K2;
    double R1=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I1*I1+K1*K1) : -sqrtf(I1*I1+K1*K1);
    double R2=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I2*I2+K2*K2) : -sqrtf(I2*I2+K2*K2);
    double xl1=z01/R1;
    double zl1=-x01/R1;
    double xl2=z02/R2;
    double zl2=-x02/R2;
    //计算转接点
    double d1=sqrt((x02-x01)*(x02-x01)+(z02-z01)*(z02-z01));
    double d2=d*(R1-R2)/d1;
    double xl=-(z02-z01)/d1;
    double zl=(x02-x01)/d1;
    double f=sqrt((R1+d)*(R1+d)-(zl*x01-xl*z01-d2)*(zl*x01-xl*z01-d2));
    double xs1=0,zs1=0;
    xs1=xl*(zl*z01+xl*x01)+d2*zl-sgn(zl*z01+xl*x01)*xl*f+x1;
    zs1=zl*(zl*z01+xl*x01)-d2*zl-sgn(zl*z01+xl*x01)*zl*f+z1;


    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));
    newElem1->setEnd(centre);

    compensationElemVector.push_back(newElem1);


}

void Compensation::Long_ArcArc_Doing(Element*elem1,Element*elem2){
    qDebug()<<"伸长型_弧弧_刀补进行";
    //获取计算公式所需要的变量
    QPointF start=dynamic_cast<myArc*>(elem1)->Start();
    QPointF end=dynamic_cast<myArc*>(elem1)->End();
    double x0=start.x(),z0=start.y();
    double x1=end.x(),z1=end.y();
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    double I1=(centre-start).x();
    double K1=(centre-start).y();
    QPointF centre2=dynamic_cast<myArc*>(elem2)->Centre();
    double I2=(centre2-end).x();
    double K2=(centre2-end).y();
    double x01=(x0+I1)-x1;
    double z01=(z0+K1)-z1;
    double x02=I2;
    double z02=K2;
    double R1=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I1*I1+K1*K1) : -sqrtf(I1*I1+K1*K1);
    double R2=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I2*I2+K2*K2) : -sqrtf(I2*I2+K2*K2);
    double xl1=-z01/R1;
    double zl1=x01/R1;
    double xl2=z02/R2;
    double zl2=-x02/R2;
    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
    zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;
    xs3=x1+d*zl2;
    zs3=z1-d*xl2;


    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));
    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));
    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);

}

void Compensation::Insert_ArcArc_Doing(Element*elem1,Element*elem2){
    qDebug()<<"插入型_弧弧_刀补进行";
    //获取计算公式所需要的变量
    QPointF start=dynamic_cast<myArc*>(elem1)->Start();
    QPointF end=dynamic_cast<myArc*>(elem1)->End();
    double x0=start.x(),z0=start.y();
    double x1=end.x(),z1=end.y();
    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    double I1=(centre-start).x();
    double K1=(centre-start).y();
    QPointF centre2=dynamic_cast<myArc*>(elem2)->Centre();
    double I2=(centre2-end).x();
    double K2=(centre2-end).y();
    double x01=(x0+I1)-x1;
    double z01=(z0+K1)-z1;
    double x02=I2;
    double z02=K2;
    double R1=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I1*I1+K1*K1) : -sqrtf(I1*I1+K1*K1);
    double R2=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I2*I2+K2*K2) : -sqrtf(I2*I2+K2*K2);
    double xl1=-z01/R1;
    double zl1=x01/R1;
    double xl2=z02/R2;
    double zl2=-x02/R2;
    //计算转接点
    double xs1=0,zs1=0,xs2=0,zs2=0,xs3=0,zs3=0,xs4=0;
    xs1=x1+d*zl1;
    zs1=z1-d*xl1;
    xs2=d*zl1+qAbs(d)*xl1+x1;
    zs2=-d*xl1+qAbs(d)*zl1+z1;
    xs3=d*zl2-qAbs(d)*xl2+x1;
    zs3=-d*xl2-qAbs(d)*zl2+z1;
    xs4=x1+d*zl2;
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End()); //起点是上条线的终点，compensationElemVector.back()指上一条线
    newElem1->setEnd(QPointF(xs1,zs1));
    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));
    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));
    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
}

void Compensation::Short_ArcArc_Building(Element*elem1,Element*elem2){
     qDebug()<<"缩短型_弧弧_刀补建立";
     QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
     QPointF start=dynamic_cast<myArc*>(elem1)->Start();
     QPointF end=dynamic_cast<myArc*>(elem1)->End();
     double x0,z0,x1,z1;
     x0=start.x(),z0=start.y(),x1=end.x(),z1=end.y();

     //计算elem1得起点向量和终点向量
     double I1=(centre-start).x();
     double K1=(centre-start).y();
     double I2=(centre-end).x();
     double K2=(centre-end).y();
     double R1=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I1*I1+K1*K1) : -sqrtf(I1*I1+K1*K1);
     double R2=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I2*I2+K2*K2) : -sqrtf(I2*I2+K2*K2);

     double xl1=K1/R1;
     double zl1=-I1/R1;
     double xl2=K2/R2;
     double zl2=-I2/R2;

     //计算两个转接点
     double xs1=x0+d*zl1;
     double zs1=z0-d*xl1;
     double xs2=x1+d*zl2;
     double zs2=z1-d*xl2;

     Line* newElem1=new Line;
     newElem1->setStart(start);
     newElem1->setEnd(QPointF(xs1,zs1));

     myArc* newElem2=new myArc;
     newElem2->setStart(QPointF(xs1,zs1)); //起点是上条线的终点，compensationElemVector.back()指上一条线
     newElem2->setEnd(QPointF(xs2,zs2));
     newElem2->setCentre(centre);

     compensationElemVector.push_back(newElem1);
     compensationElemVector.push_back(newElem2);

}

void Compensation::Short_ArcArc_End(Element*elem1,Element*elem2){
    qDebug()<<"缩短型_弧弧_刀补撤销";

    QPointF centre=dynamic_cast<myArc*>(elem1)->Centre();
    QPointF centre2=dynamic_cast<myArc*>(elem2)->Centre();
    QPointF end=dynamic_cast<myArc*>(elem1)->End();
    QPointF end2=dynamic_cast<myArc*>(elem2)->End();
    double  x1,z1,x2,z2;
    x1=end.x(),z1=end.y(),x2=end2.x(),z2=end2.y();

    //计算elem1终点向量
    double I1=(centre-end).x();
    double K1=(centre-end).y();
    double R1=dynamic_cast<myArc*>(elem1)->isArc() ? sqrtf(I1*I1+K1*K1) : -sqrtf(I1*I1+K1*K1);
    double xl1=K1/R1;
    double zl1=-I1/R1;
    //计算elem2起点向量
    double I2=(centre2-end).x();
    double K2=(centre2-end).y();
    double R2=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I2*I2+K2*K2) : -sqrtf(I2*I2+K2*K2);
    double xl2=K2/R2;
    double zl2=-I2/R2;
    //计算elem2终点向量
    double I3=(centre2-end2).x();
    double K3=(centre2-end2).y();
    double R3=dynamic_cast<myArc*>(elem2)->isArc() ? sqrtf(I2*I2+K2*K2) : -sqrtf(I2*I2+K2*K2);
    double xl3=K3/R3;
    double zl3=-I3/R3;


    //计算两个转接点
    double xs1=x1+d*zl1;
    double zs1=z1-d*xl1;
    double xs2,zs2;
    if(zl1*xl2-zl2*xl1){
        xs2=(xl2-xl1)*d/(zl1*xl2-zl2*xl1)+x1;
        zs2=(zl2-zl1)*d/(zl1*xl2-zl2*xl1)+z1;
    }else{
        xs2=x1+d*zl1;
        zs2=z1-d*xl1;
    }

    double xs3=x1+d*zl2;
    double zs3=z1-d*xl2;
    double xs4=x2+d*zl3;
    double zs4=z2-d*xl3;

    //新建4个elem
    myArc* newElem1=new myArc;
    newElem1->setStart(dynamic_cast<Shape*>(compensationElemVector.back())->End());
    newElem1->setEnd(QPointF(xs1,zs1));
    newElem1->setCentre(centre);


    Line* newElem2=new Line;
    newElem2->setStart(QPointF(xs1,zs1));
    newElem2->setEnd(QPointF(xs2,zs2));

    Line* newElem3=new Line;
    newElem3->setStart(QPointF(xs2,zs2));
    newElem3->setEnd(QPointF(xs3,zs3));

    myArc* newElem4=new myArc;
    newElem4->setStart(QPointF(xs3,zs3));
    newElem4->setEnd(QPointF(xs4,zs4));
    newElem4->setCentre(centre2);

    Line* newElem5=new Line;
    newElem5->setStart(QPointF(xs4,zs4));
    newElem5->setEnd(end2);

    compensationElemVector.push_back(newElem1);
    compensationElemVector.push_back(newElem2);
    compensationElemVector.push_back(newElem3);
    compensationElemVector.push_back(newElem4);
    compensationElemVector.push_back(newElem5);
}
