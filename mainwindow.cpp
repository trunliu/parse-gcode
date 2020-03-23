#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "graphwidget.h"
#include "codeeditwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    codeEditWidget(new CodeEditWidget)
{

//GraphWidget*graphWidget=new GraphWidget;
//stackedWidget->addWidget(graphWidget);
//stackedWidget->setCurrentWidget(0);

    createActions();
    createStatusBar();
    setLayout();
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
    //CodeEditWidget* codeEditWidget=new CodeEditWidget;
    setCentralWidget(codeEditWidget->textEdit);

}


void MainWindow::createActions()
{
    //创建一个文件菜单栏，两个工具栏
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
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


    //设置开始解析动作
    const QIcon stratIcon=QIcon::fromTheme("document-open",QIcon(":/image/images/paste.png"));
    QAction *startAct=new QAction(stratIcon,tr("&Start"),this);
    openAct->setStatusTip(tr("Begin to Parse the Gcode"));
//    QObject::connect(startAct,SIGNAL(triggered()),stackedWidget,SLOT(render()));
//    QObject::connect(startAct,SIGNAL(triggered()),stackedWidget,SLOT(showOnStatusInfo()));
    fileMenu->addAction(startAct);
    toolBar->addAction(startAct);

    return;
}

void MainWindow::newFile()
{
    return ;
}


void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()){
        loadFile(fileName);
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    //centerWidget->getCodeTextEdit()->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File loaded"), 2000);
}
