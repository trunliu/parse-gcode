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
    virtual void wheelEvent(QWheelEvent *event)override;
    void load(QString text);

signals:
public slots:

private:
    void drawElement(QPainter& painter,QPen& pen);
    void drawLine(QPainter& painter,QPen& pen,Element* it);
    void drawArc(QPainter& painter,QPen& pen,Element* it);

    void zoomIn();
    void zoomOut();
private:
    //解析用的句柄
    ParseCode* parser;

    //解析后的element容器
    QVector<Element*> elemVector;

    //放大缩小的比例系数
    double scale;
};

#endif // DRAWINGAREA_H
