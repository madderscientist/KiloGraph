#include "page.h"
#include "mainwindow.h"
#include "tdetail.h"

Page::Page(QWidget *parent)
    : QWidget{parent}
{
    mainwindow = (MainWindow*)parent;
    graph=new Graph(this);
}
void Page::resizeEvent(QResizeEvent*) {
    if(graph) graph->resize(size());
}
void Page::pointNumChange(){
    mainwindow->refreshStatus(graph->kg->v.length);
}
void Page::pointClick(Point* p,QMouseEvent* e) {
    // 按下shift拖拽: 左键连接, 右键断开
    if (e->button() == Qt::LeftButton){
        if(key_shift){
            if(p->hidechild){      // 隐藏了子节点的不准连 因为不好处理
                TipLabel::showTip("请解除收折！",parent(),500);
                return;
            }
            QDrag drag(p);
            QMimeData* data=new QMimeData();
            data->setText("connect");
            drag.setMimeData(data);
            drag.setPixmap(QPixmap(":/img/connect.png"));
            drag.exec(Qt::MoveAction);
        }
    }
    else if (e->button() == Qt::RightButton) {
        if(key_shift){
            QDrag drag(p);
            QMimeData* data=new QMimeData();
            data->setText("cut");
            drag.setMimeData(data);
            drag.setPixmap(QPixmap(":/img/cut.png"));
            drag.exec(Qt::MoveAction);
        }else{  // 弹出编辑框
            TDetail* tdetail = new TDetail(p);
            tdetail->setAttribute(Qt::WA_DeleteOnClose);
            tdetail->show();
        }
    }
}
void Page::keyPressEvent(QKeyEvent* e) {
    switch(e->key()){
    case Qt::Key_Shift:
        key_shift = true;
        break;
    }
}
void Page::keyReleaseEvent(QKeyEvent* e) {
    switch(e->key()){
    case Qt::Key_Shift:
        key_shift = false;
        break;
    }
}
