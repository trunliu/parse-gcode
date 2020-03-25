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

void DrawingArea::receiveElemVector(QVector<Element*> elemVector){
    receiveData=elemVector;
}

//将接受的数据加载到元素数组elemVector，就会自动画图
void DrawingArea::loadElemVector(){
    this->elemVector=receiveData;
}


//QPainter的使用必须paintEvent函数中，否则报错
void DrawingArea::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    QPen pen;
    painter.translate(this->width()/2,this->height()/2);

    //绘制坐标轴
    drawAxis(painter,pen);

    if(!elemVector.isEmpty()){
        drawElement(painter,pen);
    }

}

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

//鼠标滑轮事件函数
void DrawingArea::wheelEvent(QWheelEvent *event){
    // 当滚轮远离使用者时delta值为滚动的角度默认一下15°
    if(event->delta()>0){
        zoomOut();
    }else{//当滚轮向使用者方向旋转时
        zoomIn();
    }
}

void DrawingArea::zoomOut(){
    scale+=0.2;
    update();
}
void DrawingArea::zoomIn(){
    scale-=0.2;
    update();
}


void DrawingArea::mousePressEvent(QMouseEvent *event){
    //记录鼠标按下的坐标
    mousePressPos=event->pos();
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event){
     //计算鼠标移动后的坐标与之前按下鼠标的坐标偏移量
     if(event->buttons() & Qt::LeftButton){
         xoff=(event->x()- mousePressPos.x());
         yoff=(event->y()- mousePressPos.y());
         update();
     }
}

void DrawingArea::drawElement(QPainter& painter,QPen& pen){
    //it为迭代器
    for(Element* it:elemVector){
        //如果是图元类,使用多态
        if((it)->isShape()){
            //根据元素状态设置画笔颜色和类型
            if((it)->Status().isQuickLine){
                pen.setColor(Qt::white);
                pen.setStyle(Qt::DashLine);
            }else{
                if((it)->Status().isFire){
                    pen.setColor(Qt::red);
                    pen.setStyle(Qt::SolidLine);
                }else{
                    pen.setColor(Qt::green);
                    pen.setStyle(Qt::SolidLine);
                }
            }

            //判断图元类时圆弧还是直线
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
    start=dynamic_cast<Shape*>(it)->Start();
    end=dynamic_cast<Shape*>(it)->End();

    painter.setPen(pen);
    pen.setWidth(3);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    //因为画图界面的y轴正方向与坐标轴y轴方向刚好相反，所以将坐标沿x轴做反转处理
    commonFunc::reversePointByX_Axis(start);
    commonFunc::reversePointByX_Axis(end);
    //按照比例系数进行调整
    commonFunc::expandPointByScale(start,scale);
    commonFunc::expandPointByScale(end,scale);
    //按照鼠标拉动进行调整
    commonFunc::expandPointByScale(start,xoff,yoff);
    commonFunc::expandPointByScale(end,xoff,yoff);

    painter.drawLine(start,end);
}

void DrawingArea::drawArc(QPainter &painter,QPen &pen,Element* it){
    QPoint start,end,centre;
    start=dynamic_cast<Shape*>(it)->Start();
    end=dynamic_cast<Shape*>(it)->End();
    //centre=dynamic_cast<Arc*>(it)->Centre();
    painter.setPen(pen);
    pen.setWidth(3);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
}

void DrawingArea::clean(){
    if(!elemVector.isEmpty()){
        elemVector={};
    }
}
