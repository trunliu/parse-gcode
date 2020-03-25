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
    virtual void paintEvent(QPaintEvent *event)override;

    //鼠标滚轮事件
    virtual void wheelEvent(QWheelEvent *event)override;

    //鼠标按下并拖动事件
    virtual void mousePressEvent(QMouseEvent *event)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;

    void receiveElemVector(QVector<Element*> elemVector);
    void loadElemVector();
    void clean();

private:
    void drawElement(QPainter& painter,QPen& pen);
    void drawLine(QPainter& painter,QPen& pen,Element* it);
    void drawArc(QPainter& painter,QPen& pen,Element* it);
    void drawAxis(QPainter &painter,QPen &pen);

    void zoomIn();
    void zoomOut();




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
