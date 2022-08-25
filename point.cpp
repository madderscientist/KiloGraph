#include "point.h"
#include "mainwindow.h"
#include "tdetail.h"
#include "tiplabel.h"

Point::Point(QWidget *parent)
	: QPushButton{parent} {
    setAcceptDrops(true);
	this->show();
}
Point::~Point() {
    delete tdetail;
}
double Point::getZoomTime() {
	return ((Graph*)(this->parent()))->zoomTime;
}
void Point::moveF(QPointF x){
    move(x.x(),x.y());
}
void Point::setViewSize(int size) {
	int a = size ? size : RADIUS * 2 * getZoomTime();
	resize(a, a);
}
void Point::mousePressEvent(QMouseEvent *e) {
	clickpos = e->pos();
	raise();
    ((Graph*)(this->parent()))->setSelected(this);
    ((Page*)(parent()->parent()))->pointClick(this,e);  // 到page集中处理
}
void Point::mouseMoveEvent(QMouseEvent *e) {
	move(mapToParent(e->pos()) - clickpos);
//    QWidget::mouseMoveEvent(e);   不可以传递此信息！不然Graph接收到会混乱
}
void Point::mouseReleaseEvent(QMouseEvent*) {
    location = ((Graph*)(this->parent()))->mapToScene(pos());
}
void Point::mouseDoubleClickEvent(QMouseEvent*) {
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
}

void Point::dragEnterEvent(QDragEnterEvent *e) {
    if (e->source() == this)
        e->ignore();
    else
        e->accept();
}
void Point::dropEvent(QDropEvent *e) {
    if(e->mimeData()->text()=="connect"){
        ((Point*)(e->source()))->v->to(this->v);
    } else if(e->mimeData()->text()=="cut"){
        delete ((Point*)(e->source()))->v->Eto(this->v);
    } else {
        e->ignore();
        return;
    }
    e->accept();
    emit ((Page*)parent())->Page::edgeChange((Point*)(e->source()));
}

void Point::moveOnScene(int x, int y) {
    location = QPointF(x, y);
    moveF((location - ((Graph*)(this->parent()))->origin)*getZoomTime());
}
void Point::moveOnScene(QPointF x) {
	location = x;
    move(((location - ((Graph*)(this->parent()))->origin)*getZoomTime()).toPoint());
}
void Point::refreshStyle(char Selected, char Father, char Hidden, char Hidechild) {
	if (Selected != -1) selected = Selected;
	if (Father != -1) father = Father;
	if (Hidden != -1) hidden = Hidden;
	if (Hidechild != -1) hidechild = Hidechild;

	if (hidden) {
		hide();
		return;
    } else show();
	QString bgc = hidechild ? "#9BC997;" : "#FF7D94";           // 背景颜色
	QString bdc;                                                // 边框颜色
	if (selected) {
		if (father) bdc = "red";
        else bdc = "#FFC301";
	} else bdc = bgc;
	setStyleSheet(QString("border-radius:%1px;background:%2;border-color:%3").arg(width() / 2).arg(bgc, bdc));
    refreshText();
}
void Point::refreshText(){
    if(v){
        if(v->title.empty()) setText(QString::number(v->id));
        else setText(TipLabel::multiRowText(QString::fromStdString(v->title),5));
    } else setText("");
}
