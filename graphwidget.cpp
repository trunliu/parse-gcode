#include "graphwidget.h"
#include <QtWidgets>
#include "parsecode.h"
#include "element.h"
#include "commonfunc.h"
GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent),parser(new ParseCode)
{

}

void GraphWidget::paintEvent(QPaintEvent *event){

}

void GraphWidget::drawElement(){
    QVector<Element*> elemVector=parser->ParseFrom(text);

    QPen pen;
    //it为迭代器
    for(Element* it:elemVector){
        //如果是图元类
        if(it->isShape()){
            //根据元素状态设置画笔颜色类型
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


            QPoint start,end,centre;
            //设置起点终点
            start=dynamic_cast<Shape*>(it)->Start();
            end=dynamic_cast<Shape*>(it)->End();
            //如果是弧线还要设置圆心
//            if(dynamic_cast<Shape*>(it)->isArc()){
//                centre=dynamic_cast<Arc*>(elemVector[i])->Centre();
//            }

            //绘制
            QPainter painter(this);
            painter.translate(width()/2,height()/2);
            painter.setPen(pen);
            pen.setWidth(3);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setBrush(Qt::NoBrush);
            painter.drawLine(start,-end);
        }
    }
    update();
}

void GraphWidget::drawLine(){

}

void GraphWidget::drawArc(){

}
