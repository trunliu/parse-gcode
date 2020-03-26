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

    painter.setPen(pen);
    pen.setWidth(3);
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
    QPoint start,end,centre;
    int Acw=0;

    start=dynamic_cast<Shape*>(it)->Start();
    end=dynamic_cast<Shape*>(it)->End();
    //centre=dynamic_cast<Arc*>(it)->Centre();
    //Acw=dynamic_cast<Arc*>(it)->Status().isAcw;//1->逆时针 0->顺时针 -1->直线

    painter.setPen(pen);
    pen.setWidth(3);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

//    //计算参数
//    int startAngle=0, int spanAngle=0;
//    int r=sqrt((start.x()-centre.x())*(start.x()-centre.x())+(start.y()-centre.y())*(start.y()-centre.y()));
//    int width=2*r;
//    int height=2*r;
//    int x=centre.y()+r;
//    int y=centre.x()-r;
//    spanAngle=acos(向量点积/向量的模)  //弧度  /pi*180=度数

//    //起始角度=第一个向量与3点角度，3点方向为0°


//    painter.drawArc(x, y, width, height, startAngle, spanAngle);
}

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
