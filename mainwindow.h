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
QT_END_NAMESPACE
class CodeEditWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newFile();
    void open();

private:
    void createActions();
    void createStatusBar();
    void setLayout();
    void loadFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    CodeEditWidget* codeEditWidget;
};

#endif // MAINWINDOW_H
