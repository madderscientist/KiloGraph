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
void Page::pointNameChane(Point* P){
    // 更新父节点TDetail对其的连线信息
    E* e=P->v->e->tnext;
    while(e){
        TDetail* td=e->from->p->tdetail;
        if(td){
            td->clearList();
            td->iniList();
        }
        e=e->tnext;
    }
}
void Page::pointClick(Point* p,QMouseEvent* e) {
    if(tdetail){
        if(e->button() == Qt::LeftButton){
            if(p!=tdetail->P){
                tdetail->P->v->to(p->v);
                tdetail->Es->addItem(new TEdgeInf(p->v,tdetail->Es));
            } else TipLabel::showTip("不能和自己相连哦",this,600);
        } else if(e->button() == Qt::RightButton)
            setTDetail(nullptr);
        return;
    }


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
            p->tdetail = new TDetail(p, this);
            p->tdetail->show();
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
void Page::setTDetail(TDetail* t){
    if(tdetail) emit tdetail->stopAddE();
    tdetail=t;
    if(t) {
        emit t->startAddE();
        TipLabel::showTip("点击要连的点\n右击取消连线", this, 600, false);
    }
}
