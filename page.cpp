#include "page.h"
#include "mainwindow.h"
#include "tdetail.h"
#include <QMessageBox>

Page::Page(QWidget *parent, QString path)
    : QWidget{parent}, dir(path)
{
    mainwindow = (MainWindow*)parent;
    graph=new Graph(this, path);
}

void Page::resizeEvent(QResizeEvent*) {
    if(graph) graph->resize(size());
}

void Page::GraphMode(char mode){
    switch(mode){
    case 1:
        teacher = true;
        break;
    case 2:
        teacher = false;
        break;
    default:
        QMessageBox::warning(this, "warning", "文件打开失败！");
        dir="";
        break;
    }
}

void Page::pointNumChange(){
    mainwindow->refreshStatus(graph->kg->v.length);
    emit ContentChanged();
}
void Page::pointNameChange(Point* P){
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
    emit ContentChanged();
}
void Page::edgeChange(Point* from){
    // 更新父节点TDetail的边信息
    TDetail* td=from->tdetail;
    if(td){
        td->clearList();
        td->iniList();
    }
    emit ContentChanged();
}
void Page::pointClick(Point* p,QMouseEvent* e) {
    if(tdetail){
        if(e->button() == Qt::LeftButton){
            if(p!=tdetail->P){
                tdetail->P->v->to(p->v);
                tdetail->Es->addItem(new TEdgeInf(p->v,tdetail->Es));
                emit ContentChanged();
            } else TipLabel::showTip("不能和自己相连哦",this,600);
        } else
            setTDetail(nullptr);
        return;
    }
    if(TbindingV){
        if(e->button() == Qt::LeftButton){
            if(TbindingV->t->bindV(p->v)) {       // 重复添加返回false
                TbindingV->BindVs->addWidget(new BindV(p->v, TbindingV));
                emit ContentChanged();
            } else TipLabel::showTip("已经关联了哦",this,600);
        } else
            setTbindingV(nullptr);
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
            delete p->tdetail;      // 要保证析构则nullptr
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
        setTbindingV(nullptr);      // 两个全局点击只能存一
    }
}
void Page::setTbindingV(TaskInf* t){
    if(TbindingV) TbindingV->stopBindV();
    TbindingV = t;
    if(TbindingV){
        t->startBindV();
        TipLabel::showTip("点击关联的知识点\n右击取消添加", this, 600, false);
        setTDetail(nullptr);
    }
}
void Page::showTaskCard(){
    delete taskcard;
    taskcard = new TaskCard(this);
}
