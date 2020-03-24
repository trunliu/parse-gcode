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

void DrawingArea::paintEvent(QPaintEvent *event){

    QPainter painter(this);

    for(Element* it:elemVector){
        qDebug()<<it->Sentence();
    }

    drawElement(painter);

    qDebug()<<"DrawingArea::paintEvent";
}


void DrawingArea::drawElement(QPainter& painter){
    QPen pen;
    qDebug()<<"DrawingArea::drawElement";
    //it为迭代器
    for(Element* it:elemVector){
        //如果是图元类
        if(it->isShape()){
            //根据元素状态设置画笔颜色和类型
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

            //判断图元类时圆弧还是直线
            if(dynamic_cast<Shape*>(it)->isArc()){
                drawArc(painter,pen,it);
            }else{
                drawLine(painter,pen,it);
            }

        }
    }
}

void DrawingArea::drawLine(QPainter &painter,QPen pen,Element* it){
    qDebug()<<"drawLine";
    QPoint start,end;
    start=dynamic_cast<Shape*>(it)->Start();
    end=dynamic_cast<Shape*>(it)->End();
    painter.translate(width()/2,height()/2);
    painter.setPen(pen);
    pen.setWidth(3);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(start,-end);
}

void DrawingArea::drawArc(QPainter &painter,QPen pen,Element* it){
    QPoint start,end,centre;
    //如果是弧线还要设置圆心
//            if(dynamic_cast<Shape*>(it)->isArc()){
//                centre=dynamic_cast<Arc*>(elemVector[i])->Centre();
//            }

}

