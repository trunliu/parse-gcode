#ifndef PARSECODE_H
#define PARSECODE_H
#include <QObject>
#include <QVector>
#include <QPoint>
#include "commonfunc.h"

class Element;
class ParseCode
{
public:
    ParseCode();

    //解析代码入口，返回element数组
    QVector<Element*> ParseFrom(const QString &textEdit);

private:
    //解析一行代码，生成一个element对象
    Element* parseSentence(QString sentence);

    //预处理
    bool PreProcces(const QString& textEdit);

    //获取命令类型
    enum CodeType CodeType(const QString &cmd);

    //分别创建非图元，直线，弧元素
    Element* createNoShapeElement(QString sentence,QStatus status,QPointF lastPoint);
    Element* createLineElement(QString sentence,QStatus status,QPointF lastPoint);
    Element* createArcElement(QString sentence,QStatus status,QPointF lastPoint);

    //提取数值
    double extractValFrom(const QString &word);


private:
    QString text;                   //输入文本的内容

    QStringList sentenceList;       //输入文本的内容按行分解成行列表
    QStatus status;
    QPointF lastPoint;               //记录上一节末点坐标
};

#endif // PARSECODE_H
