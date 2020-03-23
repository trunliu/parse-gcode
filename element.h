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
    QPoint Start();
    QPoint End();
    void setStart(QPoint start);
    void setEnd(QPoint end);
    virtual bool isShape(){return true; }
    virtual bool isArc()=0;
protected:
    QPoint start,end;
};


class Line:public Shape
{

public:
    Line();
    virtual bool isArc(){return false;}
};


class Arc:public Shape
{
public:
    Arc();
    QPoint Centre();
    void setCentre(QPoint);
    virtual bool isArc(){return true;}
private:
    QPoint centre;
};


#endif // ELEMENT_H
