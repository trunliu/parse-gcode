#ifndef CODEEDITWIDGET_H
#define CODEEDITWIDGET_H
#include <QPlainTextEdit>
#include <QWidget>

class CodeEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CodeEditWidget(QWidget *parent = nullptr);

signals:
    void sendText(QString);

public slots:
    void sendText();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();

private:
    //读入内容
    void loadFile(const QString &fileName);

    //判断内容是否变化，是否需要保存
    bool maybeSave();

    //将内容写入到文件中
    bool saveFile(const QString &fileName);

    //设置当前文件名
    void setCurrentFile(const QString &fileName);

public:
    QPlainTextEdit* textEdit;

    //用于记录当前文件名
    QString curFile;
};

#endif // CODEEDITWIDGET_H
