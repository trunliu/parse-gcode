#include "commonfunc.h"
#include <QDebug>
commonFunc::commonFunc()
{

}


QStringList commonFunc::splitBy(QString text,QString ch){
    QStringList res;
    res=text.split(ch);
    return res;
}
