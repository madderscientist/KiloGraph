#include "tdetail.h"
#include "page.h"

TDetail::TDetail(Point* p, Page *parent)
    : Card(QString::number(p->v->id)+"号节点详情", parent), P(p)
{
    connect(Card::top->off, &QPushButton::clicked, p, [=](){p->tdetail = nullptr;});

    // 俩输入框
    title=new myLineEdit(this);
    description=new myPlainTextEdit(this);
    title->setStyleSheet("QLineEdit{font-size:22px;padding:5px 10px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                         "QLineEdit:focus{background:white;border:2px solid #90ee90;}");
    title->setText(QString::fromStdString(P->v->title));
    title->setPlaceholderText("知识点的标题");
    description->setStyleSheet("QPlainTextEdit{font-size:20px;padding:5px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                               "QPlainTextEdit:focus{background:white;border:2px solid #90ee90;}");
    description->setPlainText(QString::fromStdString(P->v->text));
    description->setPlaceholderText("知识点的具体内容\n(焦点取消自动保存)");
    description->setMaximumHeight(180);
    connect(title, &myLineEdit::focusOut, title, [=](){
        P->v->title=title->text().toStdString();
        P->refreshText();
        parent->pointNameChane(P);
    });
    connect(title, &myLineEdit::focusOut, title, [=](){
        P->v->text=description->toPlainText().toStdString();
    });

    // 仨按钮
    QPushButton* remove=new QPushButton("自删除",this);
    remove->setIcon(QIcon(":/img/delete.png"));
    remove->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                          "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(remove, &QPushButton::clicked, parent ,[=](){
        parent->graph->removeP(P);
    });
    QPushButton* addV=new QPushButton("添加分点",this);
    addV->setIcon(QIcon(":/img/add.png"));
    addV->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                        "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(addV, &QPushButton::clicked, parent ,[=](){
        Point* newP = parent->graph->addPointAtScene(QPoint(P->location.x()+30, P->location.y()-31));
        P->v->to(newP->v);
        Es->addItem(new TEdgeInf(newP->v ,Es));
        parent->graph->setSelected(P, newP);
    });
    QPushButton* addE=new QPushButton("添加边",this);
    addE->setIcon(QIcon(":/img/connect.png"));
    addE->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                        "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(addE, &QPushButton::clicked, parent ,[=](){
        if(addE->text()=="添加边") parent->setTDetail(this);
        else parent->setTDetail(nullptr);
    });

    QHBoxLayout* tools=new QHBoxLayout;
    tools->addWidget(remove);
    tools->addWidget(addV);
    tools->addWidget(addE);

    // 边列表
    Es = new QListWidget(this);
    Es->setStyleSheet("QListWidget{background:transparent;border-radius:15px;}"
                      "QListWidget::item{margin: 5px 0px;}");
    Es->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(Es,&QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item){
        TEdgeInf* Item = (TEdgeInf*)item;
        if(Item->detail->isReadOnly()) Item->expand();
        else {Item->fold();Item->detail->setPlainText(QString::fromStdString(P->v->Eto(Item->v)->text));}
    });
    connect(Es,&QListWidget::itemClicked, this, [=](QListWidgetItem *item){
        ((Page*)parent)->graph->setSelected(P, ((TEdgeInf*)item)->v->p);
    });

    Card::Layout->addWidget(title);
    Card::Layout->addWidget(description);
    Card::Layout->addLayout(tools);
    Card::Layout->addWidget(Es,1);

    // 填充列表
    iniList();
    connect(this, &TDetail::stopAddE, addE, [=](){
        parent->setCursor(Qt::CrossCursor);
        addE->setText("添加边");
    });
    connect(this, &TDetail::startAddE, addE, [=](){
        QPixmap pixmap;
        pixmap.load(":/img/connect.png");
        parent->setCursor(QCursor(pixmap.scaled(QSize(32,32), Qt::IgnoreAspectRatio)));
        addE->setText("取消添加");
    });
}
TDetail::~TDetail(){
    Page* p=(Page*)parent();
    if(p->tdetail == this)
        p->setTDetail(nullptr);
    clearList();
}
void TDetail::mousePressEvent(QMouseEvent*){
    title->clearFocus();
    description->clearFocus();
}
void TDetail::iniList(){
    E* e=P->v->e->fnext;
    while(e){
        Es->addItem(new TEdgeInf(e->to,Es));
        e = e->fnext;
    }
}
void TDetail::clearList(){
    QListWidgetItem* item = Es->takeItem(0);
    while(item){
        delete item;
        item = Es->takeItem(0);
    }
}


TEdgeInf::TEdgeInf(V* V, QListWidget *parent):
    QListWidgetItem(parent), v(V)
{
    w=new QWidget;
    w->setStyleSheet("QWidget{border-radius:15px;background:rgba(255,255,255,0.8);}"
                     "QPushButton{width:20px;height:20px;border-radius:10px;color:white;font-size:15px;}");
    Layout=new QHBoxLayout;

    ano=new QLabel(V->title.empty()?QString::number(V->id)+"号":QString::fromStdString(V->title), w);

    detail=new QPlainTextEdit(w);
    detail->setPlainText(QString::fromStdString(((TDetail*)parent->parent())->P->v->Eto(V)->text));
    detail->setReadOnly(true);

    QVBoxLayout* btns=new QVBoxLayout;

    QPushButton* del=new QPushButton("删",w);
    del->setStyleSheet("background:red;");
    w->connect(del, &QPushButton::clicked, parent, [=](){
        delete ((TDetail*)parent->parent())->P->v->Eto(V);
        parent->removeItemWidget(this);
        delete this;
    });
    cancel=new QPushButton("回",w);cancel->hide();
    cancel->setStyleSheet("background:blue;");
    w->connect(cancel, &QPushButton::clicked, parent, [=](){
        fold();
        detail->setPlainText(QString::fromStdString(((TDetail*)parent->parent())->P->v->Eto(V)->text));
    });
    save=new QPushButton("存",w);save->hide();
    save->setStyleSheet("background:green;");
    w->connect(save, &QPushButton::clicked, parent, [=](){
        fold();
        ((TDetail*)parent->parent())->P->v->Eto(V)->text = detail->toPlainText().toStdString();
    });


    btns->addWidget(del);
    btns->addWidget(cancel);
    btns->addWidget(save);

    w->setLayout(Layout);
    Layout->addWidget(ano,1);
    Layout->addWidget(detail,5);
    Layout->addLayout(btns);
    this->setSizeHint(QSize(parent->width(),60));
    parent->setItemWidget(this, w);
}
TEdgeInf::~TEdgeInf(){
    delete w;
}
void TEdgeInf::fold(){
    cancel->hide();
    save->hide();
    this->setSizeHint(QSize(listWidget()->width(),60));
    detail->setReadOnly(true);
}
void TEdgeInf::expand(){
    cancel->show();
    save->show();
    this->setSizeHint(QSize(listWidget()->width(),150));
    detail->setReadOnly(false);
}


myPlainTextEdit::myPlainTextEdit(QWidget* parent):
    QPlainTextEdit(parent){}
myLineEdit::myLineEdit(QWidget* parent):
    QLineEdit(parent){}
void myLineEdit::focusOutEvent(QFocusEvent*){emit focusOut();}
void myPlainTextEdit::focusOutEvent(QFocusEvent*){emit focusOut();}
