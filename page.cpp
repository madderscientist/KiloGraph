#include "page.h"
#include "mainwindow.h"
#include "tdetail.h"
#include "sdetail.h"
#include <QMessageBox>
#include <QFileDialog>

Page::Page(MainWindow *parent, QString path)
	: QWidget{parent}, dir(path) {
	mainwindow = parent;
	graph = new Graph(this, path);
}
Page::~Page() {
	delete student;
}
void Page::resizeEvent(QResizeEvent*) {
	if (graph) graph->resize(size());
}

void Page::GraphMode(char mode) {
	switch (mode) {
		case 1:
			teacher = true;
			break;
		case 2: {
			teacher = false;
			QTimer::singleShot(1, this, [ = ]() {
				if (QMessageBox::question(this, "提示", "是否导入进度文件？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
					QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("选择进度文件"), "/", QStringLiteral("学习进度(*.learn)"));
					if (!fileName.isEmpty())
						student = new Student(this, fileName);
				}
			});
			break;
		}
		default:
			QMessageBox::warning(this, "warning", "文件打开失败！");
			dir = "";
			break;
	}
}

void Page::pointNumChange() {
	mainwindow->refreshStatus(graph->kg->v.length);
	emit ContentChanged();
}
void Page::pointNameChange(Point* P) {
	// 更新父节点TDetail对其的连线信息
	E* e = P->v->e->tnext;
	while (e) {
		TDetail* td = (TDetail*)e->from->p->detail;
		if (td) {
			td->clearList();
			td->iniList();
		}
		e = e->tnext;
	}
	emit ContentChanged();
}
void Page::edgeChange(Point* from) {
	// 更新父节点TDetail的边信息
	TDetail* td = (TDetail*)from->detail;
	if (td) {
		td->clearList();
		td->iniList();
	}
	emit ContentChanged();
}
void Page::pointClick(Point* p, QMouseEvent* e) {
	if (teacher) {  // 老师模式
		if (tdetail) {      // 父节点正在连边
			if (e->button() == Qt::LeftButton) {
				if (p != tdetail->P) {
					tdetail->P->v->to(p->v);
					tdetail->Es->addItem(new TEdgeInf(p->v, tdetail->Es));
					emit ContentChanged();
				} else TipLabel::showTip("不能和自己相连哦", this, 600);
			} else
				setTDetail(nullptr);
			return;
		}
		if (TbindingV) {    // 题目正在绑定知识点
			if (e->button() == Qt::LeftButton) {
				if (TbindingV->t->bindV(p->v)) {      // 重复添加返回false
					TbindingV->BindVs->addWidget(new BindV(p->v, TbindingV));
					emit ContentChanged();
				} else TipLabel::showTip("已经关联了哦", this, 600);
			} else
				setTbindingV(nullptr);
			return;
		}

		// 按下shift拖拽: 左键连接, 右键断开
		if (e->button() == Qt::LeftButton) {
			if (key_shift) {
				if (p->hidechild) {    // 隐藏了子节点的不准连 因为不好处理
					TipLabel::showTip("请解除收折！", parent(), 500);
					return;
				}
				QDrag drag(p);
				QMimeData* data = new QMimeData();
				data->setText("connect");
				drag.setMimeData(data);
				drag.setPixmap(QPixmap(":/img/connect.png"));
				drag.exec(Qt::MoveAction);
			}
		} else if (e->button() == Qt::RightButton) {
			if (key_shift) {
				QDrag drag(p);
				QMimeData* data = new QMimeData();
				data->setText("cut");
				drag.setMimeData(data);
				drag.setPixmap(QPixmap(":/img/cut.png"));
				drag.exec(Qt::MoveAction);
			} else {  // 弹出编辑框
				delete p->detail;      // 要保证析构则nullptr
				p->detail = new TDetail(p, this);
				p->detail->show();
			}
		}
	}   // 老师界面:可编辑
	else {
		if (e->button() == Qt::RightButton) {
			delete p->detail;
			p->detail = new SDetail(p, this);
			int x = p->pos().x() + p->width() * 1.1;
			int y = p->pos().y() - p->detail->height() / 2.2;
			p->detail->move(x > width() ? x - p->detail->width() : x, y < 5 ? 5 : y);
			p->detail->show();
		}
	}   // 学生界面:只读

}
void Page::keyPressEvent(QKeyEvent* e) {
	switch (e->key()) {
        case Qt::Key_Shift:
			key_shift = true;
			break;
        case Qt::Key_P:         // 固定位置
            if(graph->selected) graph->selected->pin = !graph->selected->pin;
            break;
	}
}
void Page::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
		case Qt::Key_Shift:
			key_shift = false;
			break;
	}
}
void Page::setTDetail(TDetail* t) {
	if (tdetail) emit tdetail->stopAddE();
	tdetail = t;
	if (t) {
		emit t->startAddE();
		TipLabel::showTip("点击要连的点\n右击取消连线", this, 600, false);
		setTbindingV(nullptr);      // 两个全局点击只能存一
	}
}
void Page::setTbindingV(TaskInf* t) {
	if (TbindingV) TbindingV->stopBindV();
	TbindingV = t;
	if (TbindingV) {
		t->startBindV();
		TipLabel::showTip("点击关联的知识点\n右击取消添加", this, 600, false);
		setTDetail(nullptr);
	}
}
void Page::showTaskCard() {
	delete taskcard;
	taskcard = new TaskCard(this);
}
