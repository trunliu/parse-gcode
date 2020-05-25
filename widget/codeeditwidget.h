#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H
#include <QWidget>

class QPlainTextEdit;
class ParseCode;
class Element;
class CodeEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditWidget(QWidget *parent = nullptr);

public slots:
    //发送信号sendText(QString)的槽函数
    void parseCodeAndSendText();

    //新建、打开、保存、另存为槽函数
    void newFile();
    void open();
    bool save();
    bool saveAs();

signals:
    void sendElemVector(QVector<Element*>);

private slots:
    //如果编辑区内容改变，则调用该槽
    void documentWasModified();

private:
    //设置界面布局
    void layout();

    //读入内容
    void loadFile(const QString &fileName);

    //判断内容是否变化，是否需要保存
    bool maybeSave();

    //将内容写入到文件中
    bool saveFile(const QString &fileName);

    //设置当前文件名
    void setCurrentFile(const QString &fileName);

private:
    //解析用的句柄
    ParseCode* parser;

    //记录文本内容
    QPlainTextEdit* textEdit;

    //用于记录当前文件名
    QString curFile;
};

#endif // CODEEDITWIDGET_H
