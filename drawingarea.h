#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H
#include <QWidget>

class QPainter;
class Element;
class ParseCode;

class DrawingArea : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingArea(QWidget *parent = nullptr);

    //对外接口:接受elemVector数据,加载数据,清空数据
    void receiveElemVector(QVector<Element*> elemVector);
    void loadElemVector();
    void clean();

private:
    //绘图函数
    virtual void paintEvent(QPaintEvent *event)override;

    //鼠标滚轮事件
    virtual void wheelEvent(QWheelEvent *event)override;
    void zoomIn();
    void zoomOut();

    //鼠标按下并拖动事件
    virtual void mousePressEvent(QMouseEvent *event)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;

private:
    //绘制元素=画直线+画圆弧
    void drawElement(QPainter& painter,QPen& pen);
    void drawLine(QPainter& painter,QPen& pen,Element* it);
    void drawArc(QPainter& painter,QPen& pen,Element* it);

    //绘制坐标轴
    void drawAxis(QPainter &painter,QPen &pen);

private:
    //解析用的句柄
    ParseCode* parser;

    //用于数据的接受，作为中转
    QVector<Element*> receiveData;

    //解析后的element容器
    QVector<Element*> elemVector;

    //放大缩小的比例系数
    double scale;
};

#endif // DRAWINGAREA_H
