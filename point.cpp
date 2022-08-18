#include "point.h"
#include "mainwindow.h"
#include "tdetail.h"

Point::Point(QWidget *parent)
	: QPushButton{parent} {
    setAcceptDrops(true);
	this->show();
}
Point::~Point() {}
double Point::getZoomTime() {
	return ((Graph*)(this->parent()))->zoomTime;
}
void Point::moveF(QPointF x){
    move(x.x(),x.y());
}
void Point::setViewSize(int size) {
	// point的大小在Scene里面应该是固定的2RADIUS
	int a = size ? size : RADIUS * 2 * getZoomTime();
	resize(a, a);
}
void Point::mousePressEvent(QMouseEvent *e) {
	clickpos = e->pos();
	raise();
    ((Graph*)(this->parent()))->setSelected(this);
    // 按下shift拖拽: 左键连接, 右键断开
    if (e->button() == Qt::LeftButton){
        if(((Graph*)(this->parent()))->key_shift){
            QDrag drag(this);
            QMimeData* data=new QMimeData();
            data->setText("connect");
            drag.setMimeData(data);
            drag.setPixmap(QPixmap(":/img/connect.png"));
            drag.exec(Qt::MoveAction);
        }
    }
	else if (e->button() == Qt::RightButton) {
        if(((Graph*)(this->parent()))->key_shift){
            QDrag drag(this);
            QMimeData* data=new QMimeData();
            data->setText("cut");
            drag.setMimeData(data);
            drag.setPixmap(QPixmap(":/img/cut.png"));
            drag.exec(Qt::MoveAction);
        }else{  // 弹出编辑框
            TDetail* tdetail = new TDetail(this);
            tdetail->setAttribute(Qt::WA_DeleteOnClose);
            tdetail->show();
        }

	}
//    QWidget::mousePressEvent(e);    // 向Graph传递
}
void Point::mouseMoveEvent(QMouseEvent *e) {
    // 缺：收折时一起移动    ?有无必要----无必要 已用物理模型实现
	move(mapToParent(e->pos()) - clickpos);
//	((Graph*)(this->parent()))->update();
//    QWidget::mouseMoveEvent(e);   不可以传递此信息！不然Graph接收到会混乱
}
void Point::mouseReleaseEvent(QMouseEvent *e) {
	// childAt确定松手的节点
	location = ((Graph*)(this->parent()))->mapToScene(pos());   //后期做受力分析的时候需要把这行移到move上
}
void Point::mouseDoubleClickEvent(QMouseEvent *e) {
	refreshStyle(1, 1, -1, hidechild ? 0 : 1);

	queue<V*> q;
	q.push(this->v);

	int length = ((Graph*)(this->parent()))->kg->v.length;
	bool* ifvisited = new bool[length];
	for (int i = 0; i < length; i++) ifvisited[i] = false;
	ifvisited[v->id] = true;

	while (!q.empty()) {        //广度优先全部收折
		V* v = q.front();
		q.pop();
		E* e = v->e->fnext;
		while (e) {             // 把V指向的节点入队
			V* t = e->to;
			if (!ifvisited[t->id]) {
				q.push(t);
				ifvisited[t->id] = true;
				t->p->refreshStyle(0, 0, hidechild || v->p->hidechild || v->p->hidden, -1);
			}
			e = e->fnext;
		}
	}
	delete[]ifvisited;
//	((Graph*)(this->parent()))->update();
}

void Point::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->source() == this)
        e->ignore();
    else
        e->accept();
}
void Point::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->text()=="connect"){
        ((Point*)(e->source()))->v->to(this->v);
    } else if(e->mimeData()->text()=="cut"){
        delete ((Point*)(e->source()))->v->Eto(this->v);
    } else {
        e->ignore();
        return;
    }
    e->accept();
//    ((Graph*)(this->parent()))->update();
}



void Point::moveOnScene(int x, int y) {
    location = QPointF(x, y);
    moveF((location - ((Graph*)(this->parent()))->origin)*getZoomTime());
//	((Graph*)(this->parent()))->update();
}
void Point::moveOnScene(QPointF x) {
	location = x;
    move(((location - ((Graph*)(this->parent()))->origin)*getZoomTime()).toPoint());
//	((Graph*)(this->parent()))->update();
}
void Point::refreshStyle(char Selected, char Father, char Hidden, char Hidechild) {
	if (Selected != -1) selected = Selected;
	if (Father != -1) father = Father;
	if (Hidden != -1) hidden = Hidden;
	if (Hidechild != -1) hidechild = Hidechild;

	if (hidden) {
		hide();
		return;
	} else show();                      // 隐藏
	QString bgc = hidechild ? "#9BC997;" : "#FF7D94";           // 背景颜色
	QString bdc;                                                // 边框颜色
	if (selected) {
		if (father) bdc = "red";
		else bdc = "#FFDEAD";
	} else bdc = bgc;
	setStyleSheet(QString("border-radius:%1px;background:%2;border-color:%3").arg(width() / 2).arg(bgc, bdc));
}
