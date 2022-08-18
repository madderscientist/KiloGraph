#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QLabel>
#include "graph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Graph* graph;
    void refreshStatus();
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

private:
    Ui::MainWindow *ui;
    QLabel* Vnum;

};
#endif // MAINWINDOW_H
