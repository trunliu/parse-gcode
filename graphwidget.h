#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
class QScrollArea;
class ParseCode;
class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);
    virtual void paintEvent(QPaintEvent *event) override;

private:
    void drawElement();
    void drawLine();
    void drawArc();
private:
    QScrollArea* scrollArea;
    //输入
    QString text;
    //解析句柄
    ParseCode* parser;
};

#endif // GRAPHWIDGET_H
