#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "graphwidget.h"
#include "codeeditwidget.h"
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    listWidget(new QListWidget),
    stackedWidget(new QStackedWidget),
    codeEditWidget(new CodeEditWidget),
    graphWidget(new GraphWidget),
    centreWidget(new QWidget)
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

//    //设置列表窗口
//    listWidget->insertItem(0,tr("window1"));
//    listWidget->insertItem(1,tr("window2"));

//    //设置堆栈窗口
//    stackedWidget->addWidget(codeEditWidget);
//    stackedWidget->addWidget(graphWidget);

//    //设置主窗口布局
//    QGridLayout* gridLayout=new QGridLayout;
//    gridLayout->addWidget(listWidget,0,0);
//    gridLayout->addWidget(stackedWidget,0,1);

//    //设置所占比例1：6
//    gridLayout->setColumnStretch(0,1);
//    gridLayout->setColumnStretch(1,6);

//    //设置水平、垂直、以及边缘边距为10
//    gridLayout->setHorizontalSpacing(10);
//    gridLayout->setVerticalSpacing(10);
//    gridLayout->setContentsMargins(10,10,10,10);

//    connect(listWidget,SIGNAL(currentRowChanged(int)),stackedWidget,SLOT(setCurrentIndex(int)));


    //将centerWidget插入mainWindow中心
    //setCentralWidget(codeEditWidget->textEdit);
    setCentralWidget(graphWidget);

    //启动布局
    //setLayout(gridLayout);

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

    //设置保存动作
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

    //设置开始解析动作
    const QIcon stratIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/paste.png"));
    QAction *startAct=new QAction(stratIcon,tr("&Start"),this);
    openAct->setStatusTip(tr("Begin to Parse the Gcode"));
    //点击开始按钮，代码编辑界面解析代码生成elemVector数组，解析成功窗口提示信息。并发送elemVector数组至graphWidget界面
    QObject::connect(startAct,SIGNAL(triggered()),codeEditWidget,SLOT(parseCodeAndSendText()));
    //绘图界面接受代码编辑界面发送的elemVector数组
    QObject::connect(codeEditWidget,SIGNAL(sendElemVector(QVector<Element*>)),graphWidget,SLOT(receiveElemVector(QVector<Element*>)));
    graphMenu->addAction(startAct);
    toolBar->addAction(startAct);

    //设置开始绘图动作
    const QIcon graphIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/paste.png"));
    QAction *graphAct=new QAction(graphIcon,tr("&Graph"),this);
    graphAct->setStatusTip(tr("Begin to Graph"));
    //点击按钮，加载elemVector数组并画图
    QObject::connect(graphAct,SIGNAL(triggered()),graphWidget,SLOT(loadAndGraphSlot()));
    graphMenu->addAction(graphAct);
    toolBar->addAction(graphAct);

    //清除界面
    const QIcon clearIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/paste.png"));
    QAction *clearAct=new QAction(clearIcon,tr("&Clear"),this);
    graphAct->setStatusTip(tr("Clear the Broad"));
    QObject::connect(clearAct,SIGNAL(triggered()),graphWidget,SLOT(cleanSlot()));
    graphMenu->addAction(clearAct);
    toolBar->addAction(clearAct);
    return;
}



