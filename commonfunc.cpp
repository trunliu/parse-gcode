#include "commonfunc.h"

commonFunc::commonFunc()
{

}


QStringList commonFunc::splitBy(QString text,QString ch){
    QStringList res;
    text.split(ch);
    return res;
}
