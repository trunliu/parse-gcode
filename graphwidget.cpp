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
    QPalette palette=drawingBoard->palette();
    palette.setColor(QPalette::Window,Qt::black);
    drawingBoard->setPalette(palette);

    //QWidget必须初始化一个大小，否则不显示
    drawingBoard->resize(1440,800);

    //在srollArea容器中添加QWidget
    scrollArea->setWidget(drawingBoard);
}

void GraphWidget::layOut(){
    //使用格栅布局
    QGridLayout* gridLayout=new QGridLayout(this);

    //tabWidget放入第0行第0列，scrollArea放入第0行第1列
    gridLayout->addWidget(tabWidget,0,0);
    gridLayout->addWidget(scrollArea,0,1);

    //第0列比例系数为1，第1列比例系数为4，实现宽比1：4
    gridLayout->setColumnStretch(0,1);
    gridLayout->setColumnStretch(1,4);

    //设置水平、垂直、以及边缘边距为10
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setContentsMargins(10,10,10,10);

    //启动布局
    setLayout(gridLayout);

}

void GraphWidget::loadText(QString text){
    drawingBoard->load(text);
}

