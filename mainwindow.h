#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QStackedWidget;
class QListWidget;
QT_END_NAMESPACE
class CodeEditWidget;
class GraphWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

private:
    void createActions();
    void createStatusBar();
    void setLayout();
    void loadFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QListWidget* listWidget;
    QStackedWidget *stackedWidget;
    CodeEditWidget* codeEditWidget;
    GraphWidget* graphWidget;
    QDockWidget* dockWidget;
};

#endif // MAINWINDOW_H
