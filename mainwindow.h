#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QLabel>
#include <QSlider>
#include "page.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int tabIndex=-1;
    void refreshStatus(int);
    void menuBan();         // 没有文件时有些菜单用不了
    void pageSettings();

    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

    void saveAs();
    void newPage(QString path="");

    QPushButton* Pause;
    QLabel* Vnum;
    QLabel* Damping;
    QSlider* DampingSlider;

private:
    Ui::MainWindow *ui;
    int toSlider(float);
    float toDamp(int);
    void saved();               // 去掉*号

};
#endif // MAINWINDOW_H
