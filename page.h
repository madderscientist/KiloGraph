#ifndef PAGE_H
#define PAGE_H

#include "graph.h"

class MainWindow;
// 一个页面 承担多页面协同工作
class Page : public QWidget {
    Q_OBJECT
public:
    explicit Page(QWidget *parent = nullptr);
    Graph* graph=nullptr;
    QString dir;

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);

    void pointNumChange();
    void pointClick(Point*,QMouseEvent*);

    bool key_shift = false;
signals:

private:
    MainWindow* mainwindow;
};

#endif // PAGE_H
