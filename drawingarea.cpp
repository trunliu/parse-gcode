#include "drawingarea.h"
#include <QtWidgets>
#include "element.h"
#include "parsecode.h"

DrawingArea::DrawingArea(QWidget *parent) : QWidget(parent),
    parser(new ParseCode),
    elemVector(0)
{

}

//加载文本接口
void DrawingArea::load(QString text){
    elemVector=parser->ParseFrom(text);
}

//QPainter的使用必须paintEvent函数中，否则报错
void DrawingArea::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QPen pen;
    painter.translate(this->width()/2,this->height()/2);

    if(!elemVector.isEmpty()){
        drawElement(painter,pen);
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

    qDebug()<<it->Sentence();
    qDebug()<<tr("[%1,%2] [%3,%4]").arg(start.x()).arg(start.y()).arg(end.x()).arg(end.y());

    painter.setPen(pen);
    pen.setWidth(3);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    //因为画图界面的y轴正方向与坐标轴y轴方向刚好相反，所以将坐标沿x轴做反转处理
    commonFunc::reversePointByX_Axis(start);
    commonFunc::reversePointByX_Axis(end);
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

