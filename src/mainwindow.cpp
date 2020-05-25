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
    dockWidget(new QDockWidget)
{
    //使用样式表设置菜单栏颜色为两灰色(#D5D5D5十六进制)
    setStyleSheet("QMenuBar:item{background-color:#D5D5D5;}QMenuBar{background-color:#D5D5D5;}"
                  "QToolBar:item{background-color:#D5D5D5;}QToolBar{background-color:#D5D5D5;}"
                  "QStatusBar:item{background-color:#D5D5D5;}QStatusBar{background-color:#D5D5D5;}");

    //使用调色板设置背景颜色为深灰色(RGB(74,75,75)十进制)
    QPalette palette0=this->palette();
    palette0.setColor(QPalette::Background,QColor(74,75,75));
    this->setPalette(palette0);

    //程序标题
    setWindowTitle(tr("解析G代码程序"));

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

    //左侧添加停靠窗口，用于切换编辑或绘图界面
    dockWidget->setWidget(listWidget);
    addDockWidget(Qt::LeftDockWidgetArea,dockWidget);

    //设置左侧列表窗口
    listWidget->setWindowTitle(tr("界面切换栏"));
    listWidget->insertItem(0,tr("代码编辑界面"));
    listWidget->item(0)->setIcon(QIcon(":/image/icon/codeEditWidget.png"));
    listWidget->insertItem(1,tr("绘制图形界面"));
    listWidget->item(1)->setIcon(QIcon(":/image/icon/graphWidget.png"));
    //设置为大图标显示模式
    //listWidget->setViewMode(QListView::IconMode);
    connect(listWidget,SIGNAL(currentRowChanged(int)),stackedWidget,SLOT(setCurrentIndex(int)));

    //设置堆栈窗口
    stackedWidget->addWidget(codeEditWidget);
    stackedWidget->addWidget(graphWidget);

    //将stackedWidget插入mainWindow中心
    setCentralWidget(stackedWidget);

}


void MainWindow::createActions()
{
    //创建一个文件菜单栏，两个工具栏
    QMenu *fileMenu = menuBar()->addMenu(tr("文件"));
    QMenu *graphMenu = menuBar()->addMenu(tr("绘图"));
    QToolBar *fileToolBar = addToolBar(tr("文件工具栏"));
    QToolBar *toolBar = addToolBar(tr("绘图工具栏"));

    //设置新建动作,使用资源":+资源前缀名+文件名"
    const QIcon newIcon = QIcon(":/image/icon/new.png");
    QAction *newAct = new QAction(newIcon, tr("新建"), this);
    newAct->setStatusTip(tr("新建文件"));
    connect(newAct, SIGNAL(triggered()), codeEditWidget, SLOT(newFile()));
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    //设置打开动作
    const QIcon openIcon=QIcon(":/image/icon/open.png");
    QAction *openAct=new QAction(openIcon,tr("打开"),this);
    openAct->setStatusTip(tr("打开文件"));
    QObject::connect(openAct,SIGNAL(triggered()),codeEditWidget,SLOT(open()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    //设置保存动作
    const QIcon saveIcon =QIcon(":/image/icon/save.png");
    QAction *saveAct = new QAction(saveIcon, tr("保存"), this);
    newAct->setStatusTip(tr("保存文件"));
    connect(saveAct, SIGNAL(triggered()), codeEditWidget, SLOT(save()));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    //设置另存为动作
    const QIcon saveAsIcon =QIcon(":/image/icon/saveAs.png");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("另存为"), codeEditWidget, SLOT(saveAs()));
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    fileMenu->addAction(saveAsAct);

    //设置开始解析动作
    const QIcon stratIcon=QIcon(":/image/icon/start.png");
    QAction *startAct=new QAction(stratIcon,tr("开始"),this);
    openAct->setStatusTip(tr("开始解析G代码"));
    //点击开始按钮，代码编辑界面解析代码生成elemVector数组，解析成功窗口提示信息。并发送elemVector数组至graphWidget界面
    QObject::connect(startAct,SIGNAL(triggered()),codeEditWidget,SLOT(parseCodeAndSendText()));
    //绘图界面接受代码编辑界面发送的elemVector数组
    QObject::connect(codeEditWidget,SIGNAL(sendElemVector(QVector<Element*>)),graphWidget,SLOT(receiveElemVector(QVector<Element*>)));
    graphMenu->addAction(startAct);
    toolBar->addAction(startAct);

    //设置开始绘图动作
    const QIcon graphIcon=QIcon(":/image/icon/graph.png");
    QAction *graphAct=new QAction(graphIcon,tr("绘图"),this);
    graphAct->setStatusTip(tr("开始绘图"));
    //点击按钮，加载elemVector数组并画图
    QObject::connect(graphAct,SIGNAL(triggered()),graphWidget,SLOT(loadAndGraphSlot()));
    graphMenu->addAction(graphAct);
    toolBar->addAction(graphAct);

    //清除界面
    const QIcon clearIcon=QIcon(":/image/icon/clean.png");
    QAction *clearAct=new QAction(clearIcon,tr("清除"),this);
    graphAct->setStatusTip(tr("清除界面"));
    QObject::connect(clearAct,SIGNAL(triggered()),graphWidget,SLOT(cleanSlot()));
    graphMenu->addAction(clearAct);
    toolBar->addAction(clearAct);
    return;
}



