#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include <QWidget>
#include <QPixmap>
class QPlainTextEdit;
class QScrollArea;
class QTabWidget;
class Element;
class DrawingArea;


class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);

public slots:
    void receiveElemVector(QVector<Element*>);
    void loadAndGraphSlot();
    void cleanSlot();

private:
    void init();
    void layOut();

private:
    //绘图界面包含的容器
    QScrollArea* scrollArea;
    QTabWidget* tabWidget;

    //绘图设备
    DrawingArea* drawingBoard;

};

#endif // GRAPHWIDGET_H
