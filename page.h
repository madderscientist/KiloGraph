#ifndef PAGE_H
#define PAGE_H

#include "graph.h"
#include "taskcard.h"

class MainWindow;
class TDetail;
// 一个页面 承担多页面协同工作
class Page : public QWidget {
    Q_OBJECT
public:
    bool teacher = true;            // 两种模式
    void GraphMode(char mode);
    explicit Page(QWidget* = nullptr, QString="");
    Graph* graph = nullptr;
    bool physics = true;
    QString dir;

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);

    void pointNumChange();
    void pointNameChange(Point*);
    void edgeChange(Point*);
    void pointClick(Point*,QMouseEvent*);

    bool key_shift = false;
    TDetail* tdetail = nullptr;         // 此指针是用来标记那个TDetail正在addE 不参与创建TDetail
    TaskCard* taskcard = nullptr;       // 每个Page一个TaskCard展示所有题目
    TaskInf* TbindingV = nullptr;       // 此指针标记是否正在addV 不参与创建TaskInf
    void setTDetail(TDetail*);
    void setTbindingV(TaskInf*);
    void showTaskCard();

signals:
    void ContentChanged();          // 有更改的时候加星号

private:
    MainWindow* mainwindow;
};

#endif // PAGE_H
