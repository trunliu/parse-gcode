#include "graphwidget.h"
#include <QtWidgets>
#include "parsecode.h"
#include "element.h"
#include "commonfunc.h"
#include "drawingarea.h"

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent),
    scrollArea(new QScrollArea),
    tabWidget(new QTabWidget),
    drawingBoard(new DrawingArea)
{
    init();
    layOut();
}

void GraphWidget::init(){
    //必须设置运行时自动填冲
    drawingBoard->setAutoFillBackground(true);

    //获取drawingBoard的调色面板,设置成黑色
    QPalette palette1=drawingBoard->palette();
    palette1.setColor(QPalette::Window,Qt::black);
    drawingBoard->setPalette(palette1);

    //QWidget必须初始化一个大小，否则不显示
    drawingBoard->resize(1080,800);


    //获取scrollArea的调色面板,设置成黑色
    QPalette palette2=scrollArea->palette();
    palette2.setColor(QPalette::Window,Qt::darkGray);
    scrollArea->setPalette(palette2);

    //在srollArea容器中添加QWidget
    scrollArea->setWidget(drawingBoard);
}

void GraphWidget::layOut(){
    //使用格栅布局
    QGridLayout* gridLayout=new QGridLayout(this);

    //tabWidget放入第0行第0列，scrollArea放入第0行第1列
    gridLayout->addWidget(tabWidget,0,1);
    gridLayout->addWidget(scrollArea,0,0);

    //第0列比例系数为1，第1列比例系数为4，实现宽比1：4
    gridLayout->setColumnStretch(0,4);
    gridLayout->setColumnStretch(1,1);

    //设置水平、垂直、以及边缘边距为10
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setContentsMargins(10,10,10,10);

    //启动布局
    setLayout(gridLayout);

}

//接受数据的槽
void GraphWidget::receiveElemVector(QVector<Element*> elemVector){
    drawingBoard->receiveElemVector(elemVector);
}

//绘图槽:绘图区域先加载再更新界面
void GraphWidget::loadAndGraphSlot(){
    drawingBoard->loadElemVector();
    drawingBoard->update();
}

//清空界面槽:绘图区域清空数组,再更新界面
void GraphWidget::cleanSlot(){
    drawingBoard->clean();
    drawingBoard->update();
}
