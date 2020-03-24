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

signals:
public slots:
    void load(QString text);

private:
    void drawElement(QPainter& painter);
    void drawLine(QPainter &painter,QPen pen,Element* it);
    void drawArc(QPainter &painter,QPen pen,Element* it);

private:
    //解析用的句柄
    ParseCode* parser;

    //解析后的element容器
    QVector<Element*> elemVector;
};

#endif // DRAWINGAREA_H
