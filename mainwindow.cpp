#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "graphwidget.h"
#include "codeeditwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    codeEditWidget(new CodeEditWidget),
    graphWidget(new GraphWidget)
{
    setLayout();
    createActions();
    createStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createStatusBar()
{
    //初始化状态栏信息
    statusBar()->showMessage(tr("Ready"));
}


void MainWindow::setLayout()
{
    //初始化窗口大小
    resize(1442,900);

    //将centerWidget插入mainWindow中心
    //setCentralWidget(codeEditWidget->textEdit);
    setCentralWidget(graphWidget);

}


void MainWindow::createActions()
{
    //创建一个文件菜单栏，两个工具栏
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *graphMenu = menuBar()->addMenu(tr("&Graph"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    QToolBar *toolBar = addToolBar(tr("action"));

    //设置新建动作
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/image/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), codeEditWidget, SLOT(newFile()));
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    //设置打开动作
    const QIcon openIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/open.png"));
    QAction *openAct=new QAction(openIcon,tr("&Open"),this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a file"));
    QObject::connect(openAct,SIGNAL(triggered()),codeEditWidget,SLOT(open()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    //设保存动作
    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/image/images/new.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    newAct->setStatusTip(tr("save file"));
    connect(saveAct, SIGNAL(triggered()), codeEditWidget, SLOT(save()));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    //设置另存为动作
    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), codeEditWidget, SLOT(saveAs()));
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    //在代码编辑界面open一个文件时，将其文件内容同时发送给绘图界面解析。
    //QObject::connect(codeEditWidget,SIGNAL(sendText(QString)),graphWidget,SLOT(ParseGCodeSlot(QString)));

    //设置开始解析动作，点击解析动作后，将代码编辑界面的内容发送给绘图界面
    const QIcon stratIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/paste.png"));
    QAction *startAct=new QAction(stratIcon,tr("&Start"),this);
    openAct->setStatusTip(tr("Begin to Parse the Gcode"));
    //点击开始按钮，代码编辑界面发送文本内容
    QObject::connect(startAct,SIGNAL(triggered()),codeEditWidget,SLOT(sendText()));
    //绘图界面加载内容
    QObject::connect(codeEditWidget,SIGNAL(sendText(QString)),graphWidget,SLOT(loadText(QString)));
    graphMenu->addAction(startAct);
    toolBar->addAction(startAct);

    //设置开始绘图动作
    const QIcon graphIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/paste.png"));
    QAction *graphAct=new QAction(graphIcon,tr("&Graph"),this);
    graphAct->setStatusTip(tr("Begin to Graph"));
    QObject::connect(graphAct,SIGNAL(triggered()),graphWidget,SLOT(update()));
    graphMenu->addAction(graphAct);
    toolBar->addAction(graphAct);
    return;
}



