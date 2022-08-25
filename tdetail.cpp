#include "tdetail.h"
#include "page.h"

TDetail::TDetail(Point* p, Page *parent)
    : Card(QString::number(p->v->id)+"号节点详情", parent), P(p)
{
    connect(Card::top->off, &QPushButton::clicked, this, [=](){p->tdetail = nullptr;});

    // 俩输入框
    title=new myLineEdit(this);
    description=new myPlainTextEdit(this);
    title->setText(QString::fromStdString(P->v->title));
    title->setPlaceholderText("知识点的标题");
    description->setPlainText(QString::fromStdString(P->v->text));
    description->setPlaceholderText("知识点的具体内容\n(焦点取消自动保存)");
    description->setMaximumHeight(180);
    connect(title, &myLineEdit::focusOut, title, [=](){
        P->v->title=title->text().toStdString();
        P->refreshText();
        parent->pointNameChange(P);
    });
    connect(description, &myPlainTextEdit::focusOut, this, [=](){
        P->v->text = description->toPlainText().toStdString();
        emit parent->ContentChanged();
    });

    // 四按钮
    QPushButton* pin = new QPushButton(P->pin?"已固定":"固定",this);
    pin->setIcon(QIcon(":/img/pin.png"));
    pin->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                       "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(pin, &QPushButton::clicked, P ,[=](){
        pin->setText(P->pin?"固定":"已固定");
        P->pin = !P->pin;
    });
    QPushButton* remove=new QPushButton(QIcon(":/img/delete.png"),"自删除",this);
    remove->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                          "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(remove, &QPushButton::clicked, parent ,[=](){
        parent->graph->removeP(P);
    });
    QPushButton* addV=new QPushButton(QIcon(":/img/add.png"),"添加分点",this);
    addV->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                        "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(addV, &QPushButton::clicked, parent ,[=](){
        Point* newP = parent->graph->addPointAtScene(QPoint(P->location.x()+30, P->location.y()-31));
        P->v->to(newP->v);
        Es->addItem(new TEdgeInf(newP->v ,Es));
        parent->graph->setSelected(P, newP);
    });
    QPushButton* addE=new QPushButton(QIcon(":/img/connect.png"),"添加边",this);
    addE->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                        "QPushButton:pressed{background:white;border-color:#90ee90;}");
    connect(addE, &QPushButton::clicked, parent ,[=](){
        if(addE->text()=="添加边") parent->setTDetail(this);
        else parent->setTDetail(nullptr);
    });

    QHBoxLayout* tools=new QHBoxLayout;
    tools->addWidget(pin);
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

    resize(CardW,CardW/0.618);
    move(50,50);

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
