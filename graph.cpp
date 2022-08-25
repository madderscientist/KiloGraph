#include "graph.h"
#include "mainwindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>

// 物理模型参数
#define MASS 0.1                // 质量
#define DT 0.01                 // 时间间隔
#define K 6                     // 劲度系数
#define G 1e7                   // 斥力系数
#define DD 0.1                  // 斥力分母 防分母为零

Graph::Graph(Page *parent, QString path)
	: QWidget{parent}, origin(0, 0) {
    kg = new KG(path.toStdString());
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        if(parent->physics) autoMove();
        update();
    });
    show();
    QTimer::singleShot(0, this, [ = ]() {
        if(!path.isEmpty()) createByKG();
        canvas=new QPixmap(size());
        /*测试用创建节点↓*/
//        V*x = addPointAtScene(QPoint(400, 350))->v;
//        V*y = addPointAtScene(QPoint(500, 650))->v;
//        for (int i = 0; i < 4; i++)
//            x->to(addPointAtScene(QPoint(72 * i + 200, 72 * i + 300))->v);
//        for (int i = 0; i < 5; i++)
//            y->to(addPointAtScene(QPoint(70 * i + 200, 70 * i + 600))->v);
//        x->to(y);
//        x = addPointAtScene(QPoint(180, 800))->v;
//        y->to(x);
//        for (int i = 0; i < 5; i++)
//            x->to(addPointAtScene(QPoint(70 * i, 70 * i + 800))->v);
//        V*z = addPointAtScene(QPoint(500, 250))->v;
//        z->to(y);
//        setSelected(nullptr);
        /*测试用创建节点↑*/

        // 界面加载完运行. 运行顺序破大防...
        // 先执行完构造函数, 然后resizeEvent, 然后延时0秒函数, 然后paintEvent, 然后resizeEvent, 然后paintEvent

        // timer的start由mainwindow管控
    });
}
Graph::~Graph() {
	delete canvas;
    delete timer;
}
void Graph::resizeEvent(QResizeEvent *e) {
    if(canvas) delete canvas;
	canvas = new QPixmap(e->size());
}

// 根据数据结构生成点 按阿基米德螺旋排列
void Graph::createByKG() {
   Node<V*>* v=kg->v.head->next;
   float ang=1.57;
   int l=2*RADIUS;
   int a=RADIUS/3.14;
   while(v){
       ang+=(l/(a*ang));
       float r=a*ang;
       VtoP(v->data,QPoint(r*cos(ang),r*sin(ang)));
       v=v->next;
   }
}
// 根据已经有的V生成点
Point* Graph::VtoP(V* v, QPoint at){
    Point* p = plist.insert(0, new Point(this))->data;
    p->v = v;
    v->p = p;
    p->moveOnScene(at.x(), at.y());
    p->setViewSize();
    p->refreshStyle();
    return p;
};

QPoint Graph::mapToScene(QPoint view) {
	return view / zoomTime + origin;
}
QPoint Graph::mapToView(QPoint scene) {
	return (scene - origin) * zoomTime;
}

Point* Graph::addPointAtView(QPoint at) {
	return addPointAtScene(mapToScene(at));
}
Point* Graph::addPointAtScene(QPoint at) {
    // 先准备好V 再根据V生成节点
    V* v = kg->addV();
    Point* p = VtoP(v, at);
    setSelected(p);
    ((Page*)parent())->pointNumChange();
	return p;
}
void Graph::removeP(Point* p) {
	if (!p) return;
	if (selected == p) setSelected(nullptr);
	kg->removeV(p->v->id);
	plist.remove(p);
    ((Page*)parent())->pointNumChange();
//	update();
}

void Graph::drawArrow(Point* from, Point* to) {
	QPainter painter(canvas);
	QPen pen(QColor(255, 0, 1, 255));
	painter.setPen(pen);
	// 画图优化 如果不在区域内就不画----->不优化了 代价太大
	// 最初级的线 日后改为箭头
	painter.drawLine(from->x() + from->width() / 2,
	                 from->y() + from->height() / 2,
	                 to->x() + to->width() / 2,
	                 to->y() + to->height() / 2);
}
void Graph::BFS_Draw(V* start, bool* ifvisited) {
	if (start->p->hidechild) return; // 如果顶端节点是收折的父节点
	queue<V*> q;
	q.push(start);
	ifvisited[start->id] = true;

	while (!q.empty()) {
		start = q.front();      // 对每个V节点
		q.pop();
		E* e = start->e->fnext;
		while (e) {             // 把V指向的节点入队
			V* t = e->to;
			if (!ifvisited[t->id] && !t->p->hidechild) {      // 如果子节点是收折父节点则不和子节点连线
				q.push(t);
				ifvisited[t->id] = true;
			}
            if(!t->p->hidden) drawArrow(start->p, t->p);        // 如果隐藏了不画线
			e = e->fnext;
		}
	}
}
void Graph::paintEvent(QPaintEvent*) {
	canvas->fill(Qt::white);
	// 经测试, 画图事件在构造函数结束后运行, 窗口内容变化就调用重画, 但是只会更新变动的一块, 所以必须update
	// 从Qt 4.0开始，QWidget部件的所有绘制都自动使用了双缓冲，所以一般没有必要在paintEvent()函数中使用双缓冲代码来避免闪烁
	// 但是画箭头在drawArrow函数，所以用双缓冲
	bool* ifvisited = new bool[kg->v.length];
	for (int i = 0; i < kg->v.length; i++) ifvisited[i] = false;
	Node<V*>* p = kg->v.head->next;
	while (p) {
		if (!ifvisited[p->data->id] && !p->data->p->hidden) BFS_Draw(p->data, ifvisited);
		p = p->next;
	}
	delete[]ifvisited;
	QPainter painter(this);
	painter.drawPixmap(0, 0, *canvas);
}

void Graph::setSelected(Point* main, Point* son) {
	// 之前选中的还原
	if (selected) {
		selected->refreshStyle(0, 0);
		E* e = selected->v->e->fnext;
		while (e) {
			e->to->p->refreshStyle(0, 0);
			e = e->fnext;
		}
	}
    selected = main;
	if (selected) {
		selected->refreshStyle(1, 1);
        if(son){    // 单独选两个 不填就选所有子
            son->refreshStyle(1, 0);
        } else {
            E* e = selected->v->e->fnext;
            while (e) {
                e->to->p->refreshStyle(1, 0);
                e = e->fnext;
            }
        }
	}
}

void Graph::mousePressEvent(QMouseEvent *e) {
	// 右击菜单
	if (e->button() == Qt::RightButton) {
        ((Page*)parent())->setTDetail(nullptr);
        ((Page*)parent())->setTbindingV(nullptr);
		QMenu* mouseRightMenu = new QMenu(this);
		QAction* Add = mouseRightMenu->addAction("添加节点");
        QAction* TaskList = mouseRightMenu->addAction("题目列表");
        QAction* dataCenter = mouseRightMenu->addAction("转到数据中心");
        QAction* Origin = mouseRightMenu->addAction("转到世界中心");
        connect(TaskList, &QAction::triggered, (Page*)parent(), &Page::showTaskCard);
		connect(Add, &QAction::triggered, this, [ = ]() {
			addPointAtView(e->pos());
		});
        connect(dataCenter, &QAction::triggered, this, &Graph::backtoCenter);
        connect(Origin, &QAction::triggered, this, [ = ]() {
            origin = QPoint(0,0);
            refreshLocation();
        });
		mouseRightMenu->exec(cursor().pos());
		delete mouseRightMenu;  // 似乎会自动析构Arrange
	} else if (e->button() == Qt::LeftButton)
		clickpos = e->pos();
}
void Graph::mouseReleaseEvent(QMouseEvent*) {
	// 如果松开前没有移动, 则取消选中
	if (ifdrag) ifdrag = false;
    else {      // 之前是setselected(nullptr), 现改为全部遍历
        selected = nullptr;
        plist.forEach([](Node<Point*>* p, int){
            p->data->refreshStyle(0);
        });
    }
}
void Graph::mouseMoveEvent(QMouseEvent *e) {
	//上一个clickpos由pressed或上一个move修改
	moveView(clickpos, e->pos());
	clickpos = e->pos();
	ifdrag = true;
}

void Graph::refreshLocation() {
	// 根据origin做平移变换
	Node<Point*>* p = plist.head->next;
	for (int i = 0; i < plist.length; i++) {
		Point* q = p->data;
		p = p->next;
        q->moveF((q->location - origin)*zoomTime);
	}
//	update();
}
void Graph::moveView(QPoint from, QPoint to) {
	// 原点变换(scene中的)
	origin = origin + (from - to) / zoomTime;
	refreshLocation();
}
void Graph::zoom(int mode, QPoint anchor) {
	QPoint SAnchor = mapToScene(anchor);
	if (mode > 0) zoomNum++;
	else zoomNum--;
	zoomTime = qPow(1.25, zoomNum);    // zoomTime由int更新，防止出现小数丢失

	int newSize = RADIUS * 2 * zoomTime;
	// 先确定原点, 再相当于调用refreshLocation
	origin += SAnchor - mapToScene(anchor);
	// 遍历，线性变换改坐标
	Node<Point*>* p = plist.head->next;
	for (int i = 0; i < plist.length; i++) {
		Point* q = p->data;
		p = p->next;
        q->moveF((q->location - origin)*zoomTime);
		//提前算好新size
		q->resize(newSize, newSize);
		//缩放时圆角随之改变
		q->refreshStyle();
	}
//	update();
}
void Graph::wheelEvent(QWheelEvent *e) {
	//滚轮滚动，集体缩放
	zoom(e->angleDelta().y(), QPoint(e->position().x(), e->position().y()));
}

void Graph::backtoCenter() {
	// 所有点的中心
	QRect rec = childrenRect();
	origin = mapToScene((rec.topLeft() + rec.bottomRight()) / 2) - QPoint(width(), height()) / (2 * zoomTime);
	refreshLocation();
}

void Graph::autoMove() {
	Node<V*>* p = kg->v.head->next;
    QPointF* totalf = new QPointF[kg->v.length];    // 默认全零

    // 斥力 电磁模型 利用相互作用力的特点 简化为O(n^2/2)
    while (p) {    //p是被作用节点
        if(p->data->p->hidden){ p=p->next; continue;}           // 被隐藏的不算对他人的斥力
        Node<V*>* q = p->next;
        while (q) {
            QPointF vec = p->data->p->location - q->data->p->location;
            double dis = QPointF::dotProduct(vec, vec);
            QPointF Force = G * (vec) / (qPow(dis, 1.5) + DD); //指向p
            totalf[p->data->id] += Force;
            totalf[q->data->id] -= Force;
            q = q->next;
        }
        p = p->next;
    }

    // 和父节点的引力 弹簧模型
    p = kg->v.head->next;
    while (p) {
        if(p->data->p->hidden){ p=p->next; continue;}           // 被隐藏的不算对他人的引力
        E* e = p->data->e->tnext;
        while (e) {
            QPointF Force = (e->from->p->location - p->data->p->location) * K;   // 位移, 指向父节点
            totalf[p->data->id] += Force;
            totalf[e->from->id] -= Force;
            e = e->tnext;
        }
        p = p->next;
    }

	// 根据上一时刻计算 加速度 速度 位移
	p = kg->v.head->next;
	while (p) {
        if(p->data->p->hidden){ p=p->next; continue;}           // 被隐藏的不算力
        Point* P = p->data->p;
        if(P!=selected && !P->pin){
            QPointF v=P->speed;
            QPointF acceleration = (totalf[p->data->id] - DAMPING * v) / MASS;  // 改变 加速度
            P->speed += acceleration * DT;                                      // 改变 速度
            P->moveOnScene(P->location + (P->speed + v) * DT/2);                // 改变 位移
        }
		p = p->next;
	}

	delete[]totalf;
}

void Graph::keyPressEvent(QKeyEvent* e) {
    switch(e->key()){
    case Qt::Key_Delete:
        removeP(selected);
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
}

void Graph::keyReleaseEvent(QKeyEvent* e) {
    switch(e->key()){
    default:
        QWidget::keyReleaseEvent(e);
        break;
    }
}

void Graph::timerSwitch(bool on){
    timer->stop();
    if(on) timer->start(20);
}
