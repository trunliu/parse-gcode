#include "drawingarea.h"
#include <QtWidgets>
#include "element.h"
#include "parsecode.h"
#include <QMouseEvent>
#include <QMessageBox>

QPoint mousePressPos(0,0);
double xoff=0,yoff=0;

DrawingArea::DrawingArea(QWidget *parent) : QWidget(parent),
    parser(new ParseCode),
    elemVector(0),
    receiveData(0),
    scale(1)
{

}

//QPainter的使用必须paintEvent函数中，否则会报重复调用paintEvent的错误
void DrawingArea::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    QPen pen;
    painter.translate(this->width()/2,this->height()/2);

    //绘制坐标轴
    drawAxis(painter,pen);

    //当elemVector不为空时,立即绘图,所以给elemVector赋值就是绘图
    if(!elemVector.isEmpty()){
        drawElement(painter,pen);
    }

}

//建立坐标轴
void DrawingArea::drawAxis(QPainter &painter,QPen &pen){
    pen.setColor(Qt::gray);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(QPoint(0,-height()/2),QPoint(0,height()/2));
    painter.drawLine(QPoint(-width()/2,0),QPoint(width()/2,0));
}

//绘制elemVector数组中的里的每一个element
void DrawingArea::drawElement(QPainter& painter,QPen& pen){
    //it为迭代器
    for(Element* it:elemVector){
        //使用多态返回是否是直线或圆弧图元对象
        if(it->isShape()){
            //根据元素状态设置画笔颜色和类型,空移直线:白色虚线,开火线:红实线,关火线:绿实线
            if(it->Status().isQuickLine){
                pen.setColor(Qt::white);
                pen.setStyle(Qt::DashLine);
            }else{
                if(it->Status().isFire){
                    pen.setColor(Qt::red);
                    pen.setStyle(Qt::SolidLine);
                }else{
                    pen.setColor(Qt::green);
                    pen.setStyle(Qt::SolidLine);
                }
            }

            //多态判断图元类是圆弧还是直线
            if(dynamic_cast<Shape*>(it)->isArc()){
                drawArc(painter,pen,it);
            }else{
                drawLine(painter,pen,it);
            }

        }
    }
}

void DrawingArea::drawLine(QPainter& painter,QPen& pen,Element* it){
    QPoint start,end;

    //获取起始点坐标,通过动态类型转换将父类指针Element*转化成子类指针Shape*
    start=dynamic_cast<Shape*>(it)->Start();
    end=dynamic_cast<Shape*>(it)->End();

    pen.setWidth(3);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    //因为画图界面的y轴正方向与逻辑坐标轴y轴方向刚好相反，所以将坐标沿x轴做反转处理
    commonFunc::reversePointByX_Axis(start);
    commonFunc::reversePointByX_Axis(end);

    //按照滚轮滚动计算的比例系数进行调整
    commonFunc::expandPointByScale(start,scale);
    commonFunc::expandPointByScale(end,scale);

    //按照鼠标拉扯手势计算的偏移量进行调整
    commonFunc::offsetPoint(start,xoff,yoff);
    commonFunc::offsetPoint(end,xoff,yoff);

    painter.drawLine(start,end);
}

void DrawingArea::drawArc(QPainter &painter,QPen &pen,Element* it){
    QPoint start,end,center;
    int isAcw=0;
    //将it先转换成他的派生类myArc
    myArc* ArcElem=dynamic_cast<myArc*>(dynamic_cast<Shape*>(it));

    //获得：起点、终点、圆心、方向
    start=ArcElem->Start();
    end=ArcElem->End();
    center=ArcElem->Centre();
    isAcw=ArcElem->Status().isAcw;      //1->逆时针 0->顺时针 -1->直线

    pen.setWidth(3);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    //计算画弧所需的参数有3个：起始角度、夹角、外切矩形
    double startAngle=0,spanAngle=0;
    QRectF rectangle;

    //定义起始向量和终止向量
    QPoint startVector=start-center;
    QPoint endVector=end-center;

    //构建两个复数,一个是起点的复数，一个是终点的复数，两者相除即得到旋转子的复数
    //乘以一个模为1的复数时，不会导致缩放，只会产生旋转，这样的复数就称为旋转子（rotor）
    //逆时针：*旋转子(cos(θ)+sin(θ)i)   顺时针：*旋转子的共轨复数(cos(θ)-sin(θ)i)
    //默认逆时针角度为正，顺时针为负
    ComplexNum c1(startVector.x(),startVector.y());
    ComplexNum c2(endVector.x(),endVector.y());
    ComplexNum rotor=c2/c1;
    spanAngle=(isAcw) ? qAtan2(rotor.B(),rotor.A()):-qAtan2(-rotor.B(),rotor.A());

    c1.setComplexNumValue(1,0);
    c2.setComplexNumValue(startVector.x(),startVector.y());
    rotor=c2/c1;
    startAngle=qAtan2(rotor.B(),rotor.A());

    //弧度转角度
    commonFunc::radianToAngle(spanAngle);
    commonFunc::radianToAngle(startAngle);

    //最后配置外切的矩形,还需要考虑放大的系数和矩形坐标轴的反转。弧线的半径=向量的长度
    //放大得时候,角度值都不会发送变化,但圆心得位置和半径会变。
    int r=startVector.manhattanLength()*scale;
    rectangle.setRect(center.x()-r,-(center.y()+r),2*r,2*r);

    //由于16个像素相当于1°，所以所求角度还需*16
    painter.drawArc(rectangle, startAngle*16, spanAngle*16);
}

//接受数据
void DrawingArea::receiveElemVector(QVector<Element*> elemVector){
    receiveData=elemVector;
}

//将接受的数据加载到元素数组elemVector，就会自动画图
void DrawingArea::loadElemVector(){
    this->elemVector=receiveData;
}

//鼠标滚轮事件函数
void DrawingArea::wheelEvent(QWheelEvent *event){
    // 当滚轮远离使用者时delta值为滚动的角度默认一下15°
    if(event->delta()>0){
        zoomOut();
    }else{//当滚轮向使用者方向旋转时
        zoomIn();
    }
}

//滚轮滚动一格，放大0.2倍
void DrawingArea::zoomOut(){
    scale+=0.2;
    update();
}

//滚轮滚动一格，缩小0.2倍
void DrawingArea::zoomIn(){
    scale-=0.2;
    update();
}

//记录鼠标按下的坐标
void DrawingArea::mousePressEvent(QMouseEvent *event){ 
    mousePressPos=event->pos();
}

//计算鼠标移动后的坐标与之前按下鼠标的坐标偏移量
void DrawingArea::mouseMoveEvent(QMouseEvent *event){   
     if(event->buttons() & Qt::LeftButton){
         xoff=(event->x()- mousePressPos.x());
         yoff=(event->y()- mousePressPos.y());
         update();
     }
}

//清理界面=清空elemVector数组
void DrawingArea::clean(){
    if(!elemVector.isEmpty()){
        elemVector={};
    }
}
