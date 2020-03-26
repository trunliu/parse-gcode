#include "codeeditwidget.h"
#include <QtWidgets>
#include "parsecode.h"
#include "element.h"

CodeEditWidget::CodeEditWidget(QWidget *parent) :
    QWidget(parent),
    textEdit(new QPlainTextEdit),
    curFile(),
    parser(new ParseCode)
{
    layout();
    //如果文件内容变化，则调用槽documentWasModified()
    connect(textEdit->document(), SIGNAL(contentsChanged()),this, SLOT(documentWasModified()));
}


void CodeEditWidget::layout(){
    QGridLayout* gridLayout=new QGridLayout;
    gridLayout->addWidget(textEdit);
    setLayout(gridLayout);
}

void CodeEditWidget::documentWasModified(){
    setWindowModified(textEdit->document()->isModified());
}

//sendText()槽用于发送信号，信号包含了文本内容，在绘图界面graphWidget设置接受信号数据的槽函数
void CodeEditWidget::parseCodeAndSendText(){
    QString text=textEdit->toPlainText();

    //当编辑区文本不为空时才解析并发送
    if(!text.isEmpty()){
        QVector<Element*> elemVector=parser->ParseFrom(text);
        if(elemVector.isEmpty()){
            qDebug()<<"解析失败";
            QMessageBox::information(this,"提示","解析代码失败",QMessageBox::Yes);
            return;
        }else{
            qDebug()<<"解析成功";
            QMessageBox::information(this,"提示","解析代码成功",QMessageBox::Yes);
            emit sendElemVector(elemVector);
        }
    }
}

void CodeEditWidget::open(){
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,"打开","./",
                                                        "焊接离线文件 (*.txt *.cnc)");
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

//保存槽 如果没有文件路径则调用另存为槽，如果有则保存到文件路径中
bool CodeEditWidget::save(){
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

//另存为槽，呼出窗口QFileDialog，模式为setAcceptMode另存为窗口
bool CodeEditWidget::saveAs(){
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    //窗口运行只要没有点击确定按钮就取消窗口
    if (dialog.exec() != QDialog::Accepted)
        return false;

    //调用真正得保存函数，保存至第一个旋转的文件名中
    return saveFile(dialog.selectedFiles().first());
}

//new之前先判断内容是否改变，是否需要保存
void CodeEditWidget::newFile(){
    //等maybeSave()处理完，就清理textEdit，否则啥也不干
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}




//正常都返回true，表示处理完成，false表示取消，啥都不干
bool CodeEditWidget::maybeSave(){
    //若内容没变直接返回也不需要保存
    if (!textEdit->document()->isModified()){
        return true;
    }
    //否则弹窗
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("提示"),
                               tr("内容已被修改.\n"
                                  "是否保存文件?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    //选择保存按钮
    case QMessageBox::Save:
        return save();
    //选择取消按钮
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool CodeEditWidget::saveFile(const QString &fileName){
    QFile file(fileName);

    //打开不成功提示warining
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("提示"),
                             tr("没有保存的文件路径 %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    //通过管道写入到绑定的文件中
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    setCurrentFile(fileName);
    return true;
}

void CodeEditWidget::loadFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("提示"),
                             tr("读取文件失败 %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }
    QTextStream in(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    textEdit->setPlainText(in.readAll());

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

//设置当前所处的文件
void CodeEditWidget::setCurrentFile(const QString &fileName){
    curFile = fileName;
    textEdit->document()->setModified(false);
    //setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";

    //文件路径与窗口关联，指定路径设为文件名。
    //setWindowFilePath(shownName);
}
