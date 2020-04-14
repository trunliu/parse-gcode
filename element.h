#ifndef ELEMENT_H
#define ELEMENT_H
#include <QPoint>
#include <QObject>
#include "commonfunc.h"

class Element
{
public:
    Element();
    QString Sentence();
    QStatus Status();
    void setSentence(QString);
    void setStatus(QStatus status);
    virtual bool isShape()=0;

protected:
    QStatus status;
    QString sentence;
};


class NoShape:public Element
{
public:
    NoShape();
    virtual bool isShape(){return false; }
};


class Shape:public Element
{
public:
    Shape();
    QPointF Start();
    QPointF End();
    void setStart(QPointF start);
    void setEnd(QPointF end);
    virtual bool isShape(){return true; }
    virtual bool isArc()=0;
protected:
    QPointF start,end;
};


class Line:public Shape
{
public:
    Line();
    virtual bool isArc(){return false;}
};


class myArc:public Shape
{
public:
    myArc();
    QPointF Centre();
    void setCentre(QPointF);
    virtual bool isArc(){return true;}
private:
    QPointF centre;
};


#endif // ELEMENT_H
