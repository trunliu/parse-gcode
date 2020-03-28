#include "element.h"

Element::Element():status(),sentence(" "){}

QString Element::Sentence()
{
    return this->sentence;
}

QStatus Element::Status()
{
    return this->status;
}

void Element::setSentence(QString sentence)
{
    this->sentence=sentence;
}

void Element::setStatus(QStatus status)
{
    this->status=status;
}

/************************************************/
NoShape::NoShape(){}

Shape::Shape():Element(),start(0,0),end(0,0){}

QPoint Shape::Start()
{
    return this->start;
}

QPoint Shape::End()
{
    return this->end;
}

void Shape::setStart(QPoint start)
{
    this->start=start;
}

void Shape::setEnd(QPoint end)
{
    this->end=end;
}
/************************************************/

Line::Line(){}


myArc::myArc():centre(0,0){}

QPoint myArc::Centre()
{
    return this->centre;
}

void myArc::setCentre(QPoint centre)
{
    this->centre=centre;
}
