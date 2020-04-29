#ifndef COMPENSATION_H
#define COMPENSATION_H
#include <QObject>
#include <QVector>
const double PI=3.1415926;

//转接类型：缩短型、伸长型、插入型
enum transType{
    SHORTER,LONGER,INSERT
};

//时间类型：刀补进行、刀补建立、刀补撤销
enum timeType{
    DOING,BUILDING,CANCEL
};

//连接元素类型：线线型、线弧型、弧线型、弧弧型
enum elemType{
  LINE_LINE,LINE_ARC,ARC_LINE,ARC_ARC
};

class Element;
class Compensation
{
public:
    Compensation();
    //输入数据
    void setVector(const QVector<Element*>& elemVector);

    //开始刀补计算
    QVector<Element*> calulate(double d);

private:
    //运行计算得主函数
    void run(int beginIndex,int endIndex);

    //判断转接类型
    enum transType transferType(Element*elem1,Element*elem2);

    //判断连接的元素类型
    enum elemType elemsType(Element*elem1,Element*elem2);

    //选择最终计算所用得公式算法
    void selectAlgorithm(Element*elem1,Element*elem2,transType trantyp,elemType elemtyp,timeType timetyp);

    //计算方向矢量
    QPointF directionVector(Element*elem,int pos);

private:
    //输入
    QVector<Element*> elemVector;
    //刀具半径
    double d;
    //输出
    QVector<Element*> compensationElemVector;

private:
    void Long_LineLine_Building(Element*elem1,Element*elem2);
      void Long_LineLine_End(Element*elem1,Element*elem2);
      void Long_LineLine_Doing(Element*elem1,Element*elem2);

      void Short_LineLine_Building(Element*elem1,Element*elem2);
      void Short_LineLine_Doing(Element*elem1,Element*elem2);
      void Short_LineLine_End(Element*elem1,Element*elem2);

      void Insert_LineLine_Building(Element*elem1,Element*elem2);
      void Insert_LineLine_End(Element*elem1,Element*elem2);
      void Insert_LineLine_Doing(Element*elem1,Element*elem2);

      void Short_LineArc_Building(Element*elem1,Element*elem2);
      void Short_LineArc_Doing(Element*elem1,Element*elem2);

      void Long_LineArc_Building(Element*elem1,Element*elem2);
      void Long_LineArc_Doing(Element*elem1,Element*elem2);

      void Insert_LineArc_Building(Element*elem1,Element*elem2);
      void Insert_LineArc_Doing(Element*elem1,Element*elem2);

      void Short_ArcLine_Doing(Element*elem1,Element*elem2);
      void Short_ArcLine_End(Element*elem1,Element*elem2);

      void Long_ArcLine_Doing(Element*elem1,Element*elem2);
      void Long_ArcLine_End(Element*elem1,Element*elem2);

      void Insert_ArcLine_Doing(Element*elem1,Element*elem2);
      void Insert_ArcLine_End(Element*elem1,Element*elem2);

      void Short_ArcArc_Doing(Element*elem1,Element*elem2);
      void Long_ArcArc_Doing(Element*elem1,Element*elem2);
      void Insert_ArcArc_Doing(Element*elem1,Element*elem2);

      void Short_ArcArc_Building(Element*elem1,Element*elem2);
      void Short_ArcArc_End(Element*elem1,Element*elem2);

};

#endif // COMPENSATION_H
