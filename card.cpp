#include "card.h"

TopBar::TopBar(QString title, QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setCursor(Qt::OpenHandCursor);
    setStyleSheet("TopBar{border-top-left-radius:20px;border-top-right-radius:20px;background:rgb(255,154,202);}"
                  "QLabel{font-size:30px;font-weight:bold;background:transparent;}"
                  "QPushButton{width:20px;height:20px;background:red;border-radius:10px;}");

    QHBoxLayout* topLayout=new QHBoxLayout;
    topLayout->setContentsMargins(CardRadius,0,10,0);
    setLayout(topLayout);

    Title = new QLabel(title,this);
    topLayout->addWidget(Title);

    topLayout->addStretch(1);

    off=new QPushButton(this);
    off->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(off);
    connect(off, &QPushButton::clicked, parent, &Card::close);
}
void TopBar::setTitie(QString title){
    Title->setText(title);
}
void TopBar::mousePressEvent(QMouseEvent* e){
    setCursor(Qt::ClosedHandCursor);
    clickpos=e->pos();
}
void TopBar::mouseReleaseEvent(QMouseEvent*){
    setCursor(Qt::OpenHandCursor);
}
void TopBar::mouseMoveEvent(QMouseEvent* e){
    Card* p=(Card*)parent();
    QPoint newpos=p->mapToParent(e->pos()) - clickpos;
    if(newpos.x()>=0 && newpos.y()>=0)
        p->move(newpos);
}



BottomResize::BottomResize(QWidget *parent)
    : QWidget{parent}
{
    resize(20,20);
    setCursor(Qt::SizeFDiagCursor);
}
void BottomResize::mouseMoveEvent(QMouseEvent* e){
    Card* p=(Card*)parent();
    QPoint x= mapToParent(e->pos());
    p->resize(x.x(),x.y());
}



Card::Card(QString Title, QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setStyleSheet("Card{border-radius:20px;background:rgba(255,154,202,0.75);}");
    setMinimumSize(170,230);
    setCursor(Qt::ArrowCursor);

    top=new TopBar(Title, this);
    top->move(0,0);

    bottom=new BottomResize(this);

    Layout = new QVBoxLayout;
    Layout->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    Layout->setContentsMargins(20,50,20,20);
    setLayout(Layout);
}
void Card::resizeEvent(QResizeEvent* e){
    top->resize(e->size().width(), 2*CardRadius);
    bottom->move(width()-bottom->width(),height()-bottom->height());
}
void Card::niceButton(QPushButton* btn){
    btn->setStyleSheet("QPushButton{border:2px solid red;height:35px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                       "QPushButton:pressed{background:white;border-color:#90ee90;}");
}
void Card::niceLineEdit(QLineEdit* le){
    le->setStyleSheet("QLineEdit{font-size:22px;padding:5px 10px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                      "QLineEdit:focus{background:white;border:2px solid #90ee90;}");
}

myPlainTextEdit::myPlainTextEdit(QWidget* parent):
    QPlainTextEdit(parent){
    setStyleSheet("QPlainTextEdit{font-size:20px;padding:5px;background:rgba(255,255,255,0.8);border-radius:15px;}"
                                   "QPlainTextEdit:focus{background:white;border:2px solid #90ee90;}");
}
myLineEdit::myLineEdit(QWidget* parent):
    QLineEdit(parent){
    Card::niceLineEdit(this);
}
void myLineEdit::focusOutEvent(QFocusEvent*){emit focusOut();}
void myPlainTextEdit::focusOutEvent(QFocusEvent*){emit focusOut();}
