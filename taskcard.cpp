#include "taskcard.h"
#include "page.h"

TaskCard::TaskCard(Page *parent)
    : Card("题目列表", parent)
{
    QPushButton* addT=new QPushButton(QIcon(":/img/add.png"),"添加题目", this);
    addT->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                            "QPushButton:pressed{background:white;border-color:#90ee90;}");
    QPushButton* importT=new QPushButton("导入题目", this);
    importT->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                            "QPushButton:pressed{background:white;border-color:#90ee90;}");
    QPushButton* exportT=new QPushButton("导出题目", this);
    exportT->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                            "QPushButton:pressed{background:white;border-color:#90ee90;}");

    QHBoxLayout* tools = new QHBoxLayout;
    tools->addWidget(addT);
    tools->addWidget(importT);
    tools->addWidget(exportT);

    Ts=new QListWidget(this);
    Ts->setStyleSheet("QListWidget{background:transparent;border-radius:15px;}"
                      "QListWidget::item{margin: 5px 0px;}");
    Ts->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    Card::Layout->addLayout(tools);
    Card::Layout->addWidget(Ts);

    resize(2*CardW,2*CardW*0.618);
    move(parent->width()/2-CardW, 10);
    show();
    iniList();

    connect(addT, &QPushButton::clicked, this, [=](){
        Ts->insertItem(0,new TaskInf(parent->graph->kg->addTask(""), this, Ts));
        emit parent->ContentChanged();
    });
    connect(importT, &QPushButton::clicked, this, [=](){});
    connect(exportT, &QPushButton::clicked, this, [=](){});
    connect(Ts,&QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item){
        TaskInf* Item = (TaskInf*)item;
        Item->ifFold(Item->taskText->isEnabled());
    });
}
TaskCard::~TaskCard(){
    clearList();
    ((Page*)parent())->taskcard = nullptr;
}

void TaskCard::iniList(){
    Page* page=(Page*)parent();
    Node<Task*>* p = page->graph->kg->task.head->next;
    while(p){
        Ts->addItem(new TaskInf(p->data, this, Ts));
        p=p->next;
    }
}
void TaskCard::clearList(){
    QListWidgetItem* item = Ts->takeItem(0);
    while(item){
        delete item;
        item = Ts->takeItem(0);
    }
}


TaskInf::TaskInf(Task* Task,TaskCard* parent, QListWidget* List):
    QListWidgetItem(List),page((Page*)parent->parent()), t(Task)
{
    w=new QWidget(parent);
    w->setStyleSheet("QWidget{border-radius:15px;background:rgba(255,255,255,0.8);}");
    QVBoxLayout* Layout=new QVBoxLayout(w);               // 主垂直布局

    QLabel* IDlabel = new QLabel(QString("编号%1").arg(Task->id), w);
    IDlabel->setStyleSheet("background:transparent;");

    layout1=new QHBoxLayout;       // 放答案和删除
    layout2=new QHBoxLayout;       // 最后一行

    taskText = new myPlainTextEdit(w);
    taskText->setPlaceholderText("题干(双击编辑 焦点转移自动保存)");

    ans = new myLineEdit(w);
    ans->setPlaceholderText("题目答案");

    QScrollArea* scrollArea =new QScrollArea(w);           // 放BindV们
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget* scrollWidget=new QWidget(w);
    BindVs=new QHBoxLayout(scrollWidget);
    BindVs->setMargin(0);
    BindVs->setAlignment(Qt::AlignLeft);
    scrollArea->setWidget(scrollWidget);

    QPushButton* off=new QPushButton("删除",w);
    off->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                       "QPushButton:pressed{background:white;border-color:#90ee90;}");

    QPushButton* addV=new QPushButton(QIcon(":/img/add.png"), "关联知识点", w);
    addV->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                        "QPushButton:pressed{background:white;border-color:#90ee90;}");

    // 布局
    layout1->addWidget(ans,1);
    layout1->addWidget(off);
    layout2->addWidget(scrollArea,1);
    layout2->addWidget(addV);
    Layout->addWidget(IDlabel);
    Layout->addWidget(taskText);
    Layout->addLayout(layout1);
    Layout->addLayout(layout2);
    readTask();
    ifFold(true);
    List->setItemWidget(this, w);

    w->connect(off, &QPushButton::clicked, parent, [=](){
        emit page->ContentChanged();
        page->graph->kg->removeTask(Task->id);
        List->removeItemWidget(this);
        delete this;
    });
    w->connect(taskText, &myPlainTextEdit::focusOut, parent, [=](){saveTask();});
    w->connect(ans, &myLineEdit::focusOut, parent, [=](){saveTask();});
    w->connect(addV, &QPushButton::clicked, [=](){
        if(page->TbindingV != this) page->setTbindingV(this);
        else page->setTbindingV(nullptr);
    });
}
TaskInf::~TaskInf(){
    if(page->TbindingV==this) page->setTbindingV(nullptr);
    delete w;
}

void TaskInf::ifFold(bool b){
    taskText->setDisabled(b);
    ans->setDisabled(b);
    for (int i = 0; i < layout1->count(); ++i) {
        QWidget* w = layout1->itemAt(i)->widget();
        if (w) w->setVisible(!b);
    }
    for (int i = 0; i < layout2->count(); ++i) {
        QWidget* w = layout2->itemAt(i)->widget();
        if (w) w->setVisible(!b);
    }
    this->setSizeHint(QSize(listWidget()->width(),b ? 100:300));
    if(b) clearVs();else iniVs();
}

void TaskInf::readTask(){
    QString x(QString::fromStdString(t->text));
    int i=x.indexOf(DIVIDE);
    taskText->setPlainText(x.left(i));
    if(i>-1) ans->setText(x.mid(i+3));
}
void TaskInf::saveTask(){
    t->text = (taskText->toPlainText() + DIVIDE + ans->text()).toStdString();
}
void TaskInf::clearVs(){
    for (int i = 0; i < BindVs->count(); ++i) {
        QWidget* w = BindVs->itemAt(i)->widget();
        if(w) delete w;
    }
}
void TaskInf::iniVs(){
    Node<V*>* p = t->v.head->next;
    while(p){
        BindVs->addWidget(new BindV(p->data, this));
        p=p->next;
    }
}
void TaskInf::startBindV(){
    QPixmap pixmap;
    pixmap.load(":/img/connect.png");
    page->setCursor(QCursor(pixmap.scaled(QSize(32,32), Qt::IgnoreAspectRatio)));
}
void TaskInf::stopBindV(){
    page->setCursor(Qt::CrossCursor);
}



BindV::BindV(V* V, TaskInf* parent):
    QWidget(parent->listWidget()->parentWidget()), v(V), page(parent->page)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setStyleSheet("height:36px;width:140px;border-radius:18px;background:yellow;margin:9px;");
    setMaximumWidth(140);
    QHBoxLayout* Layout = new QHBoxLayout(this);

    QLabel* vname = new QLabel(V->title.empty()?QString::number(V->id)+"号":QString::fromStdString(V->title), this);
    vname->setStyleSheet("background:transparent;font-size:16px;");

    QPushButton* off = new QPushButton("×",this);
    off->setStyleSheet("height:18px;width:18px;border-radius:9px;background:red;");

    Layout->addWidget(vname,1);
    Layout->addWidget(off);
    show();

    connect(off, &QPushButton::clicked, this, [=](){
        emit page->ContentChanged();
        parent->t->unbindV(V);
        close();
    });
}
void BindV::mousePressEvent(QMouseEvent*){
    page->graph->setSelected(v->p);
}
