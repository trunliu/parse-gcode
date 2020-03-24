#include "parsecode.h"
#include "element.h"
#include "QDebug"

//构造函数，用列表的方式对成员进行初始化
ParseCode::ParseCode():text(" "),
    sentenceList(QStringList(0))
{                  
//    QStatus status;
//    QPoint lastPoint;
}


//解析代码入口，返回element数组
QVector<Element*> ParseCode::ParseFrom(const QString &textEdit){
    Element*tmpElement=NULL;
    QVector<Element*> elemList;

    //预处理：将文本内容分割成一行一行的存入sentenceList列中
    if(!PreProcces(textEdit)){
        qDebug()<<"文件为空..."<<endl;
        return elemList;
    }
    qDebug()<<"sentenceList.size:"<<sentenceList.size();

    //使用堆内存返回指针是正确的，但是注意可能产生内存泄露问题，在使用完毕后主函数中释放该段内存
    //遍历每一行并解析每一行
    for(int row=0;row<sentenceList.length();row++)
    {
        //tmpElement=parseSentence(sentenceList[row]);
        if(tmpElement){
            elemList.push_back(tmpElement);
        }
    }

    for(int i=0;i<elemList.size();++i){
        qDebug()<<elemList[i]->Sentence();
    }
    delete tmpElement;
    return elemList;
}

//预处理
bool ParseCode::PreProcces(const QString &textEdit){
    this->text=textEdit;
    if(textEdit.size()){
        //去前后空格
        text=text.trimmed();

        //全部大写
        text=text.toUpper();

        //按换行符\n分割文本内容
        sentenceList=commonFunc::splitBy(text,"\n");
        return true;
    }else{
        return false;
    }
}

//解析一行代码，生成一个element对象
//因为Element是纯虚函数，无法实例new出来，所以采用二重指针（指针的指针），res里放着指向Element的指针的地址
Element* ParseCode::parseSentence(QString sentence){
    Element** res=new Element*;

    //每一行进行预处理,两头去空格
    sentence=sentence.trimmed();

    //提取指令参数
    QString cmd=sentence.left(3);

    //根据指令类型更新状态信息，然后创建元素
    switch(CodeType(cmd))
    {
        case G99_CODE:break;
        case RELATIVE_CODE:{
            status.isRelative=true;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case ABSOLUTE_CODE:{
            status.isRelative=false;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case INCH_CODE:{
            status.isMeter=true;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case METER_CODE:{
            status.isMeter=false;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case L_KERF_CODE:{
            status.isLeft=1;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case R_KERF_CODE:{
            status.isLeft=0;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case CANCEL_KERF_CODE:{
            status.isLeft=-1;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case FIRE_ON_CODE:{
            status.isFire=true;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case FIRE_OFF_CODE:{
            status.isFire=false;
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case FINISH_CODE:{
            *res=createNoShapeElement(sentence,status,lastPoint);
            break;
       }
        case QUICK_LINE_CODE:{
            status.isQuickLine=true;
            *res=createLineElement(sentence,status,lastPoint);
            break;
       }
        case LINE_CODE:{
            status.isQuickLine=false;
            *res=createLineElement(sentence,status,lastPoint);
            break;
       }
        case ACW_ARC_CODE:{
            status.isAcw=1;
            *res=createArcElement(sentence,status,lastPoint);
            break;
       }
        case CW_ARC_CODE:{
            status.isAcw=0;
            *res=createArcElement(sentence,status,lastPoint);
            break;
       }default:{
           qDebug()<<"未知命令";
           break;
       }
    }

    //如果是图元类则将此元素的终点记录下来，作为下一个元素的起点使用
    if((*res)->isShape()){
        lastPoint=static_cast<Shape*>(*res)->End();
    }
    return *res;
}

//获取命令类型
enum CodeType ParseCode::CodeType(const QString &cmd){
    enum CodeType res;
    if(cmd=="G99")//部件选项
        res=G99_CODE;
    else if(cmd=="G91")//相对坐标
        res=RELATIVE_CODE;
    else if(cmd=="G90")//绝对坐标
        res=ABSOLUTE_CODE;
    else if(cmd=="G20")//英制
        res=INCH_CODE;
    else if(cmd=="G21")//米制
        res=METER_CODE;
    else if(cmd=="G41")//左割缝补偿
        res=L_KERF_CODE;
    else if(cmd=="G42")//右割缝补偿
        res=R_KERF_CODE;
    else if(cmd=="G40")//割缝补偿取消
        res=CANCEL_KERF_CODE;
    else if((cmd=="G00") || cmd=="G0")//空车快移
        res=QUICK_LINE_CODE;
    else if((cmd=="G01") || cmd=="G1")//直线切割
        res=LINE_CODE;
    else if((cmd=="G02") || cmd=="G2")//顺时针圆弧
        res=CW_ARC_CODE;
    else if(cmd=="G03" || cmd=="G3")//逆时针圆弧
        res=ACW_ARC_CODE;
    else if(cmd=="M07" || cmd=="M17")
        res=FIRE_ON_CODE;
    else if(cmd=="M08" || cmd=="M18")
        res=FIRE_OFF_CODE;
    else if(cmd=="M02")
        res=FINISH_CODE;
    else//处理其他情况
        res=NO_USE_CODE;
    return res;
}

//创建非图元类型的元素
Element* ParseCode::createNoShapeElement(QString sentence,QStatus status,QPoint lastPoint){
    //不需要lastPoint这个参数
    Q_UNUSED(lastPoint);
    NoShape*noShapeElement=new NoShape;

    //配置非图元类的元素，只需要状态和内容信息
    noShapeElement->setStatus(status);
    noShapeElement->setSentence(sentence);
    return noShapeElement;
}

//创建直线元素
Element* ParseCode::createLineElement(QString sentence,QStatus status,QPoint lastPoint){
    //对一行进行分割，按空格分割成单词列
    QStringList words=commonFunc::splitBy(sentence," ");
    Line* lineElement=new Line;
    QPoint end(0,0);
    double xoff=0,yoff=0;

    //提取数据
    xoff=extractValFrom(words[1]);
    yoff=extractValFrom(words[2]);

    //相对坐标情况下，结果为增量式坐标
    if(status.isRelative)
    {
        end.setX(lastPoint.x()+xoff);
        end.setY(lastPoint.y()+yoff);
    }else{
        end.setX(xoff);
        end.setY(yoff);
    }

    //配置element，上一个线的终点lastPoint就是这条线的起点。
    lineElement->setStatus(status);
    lineElement->setEnd(end);
    lineElement->setStart(lastPoint);
    lineElement->setSentence(sentence);
    return lineElement;
}

//创建弧元素
Element* ParseCode::createArcElement(QString sentence,QStatus status,QPoint lastPoint){
    Arc* res=new Arc;

    return res;
}




//提取数值
double ParseCode::extractValFrom(const QString &word){
    double res=0;

    //没有参数
    if(word.length()<2){
        qDebug()<<"没有参数";
        return res;
    }

    res=word.right(word.length()-1).toDouble();
    return res;
}
