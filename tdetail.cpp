#include "tdetail.h"
#include "ui_tdetail.h"
#include "graph.h"
#include <QString>
#include <QDebug>

TDetail::TDetail(Point*P, QWidget *parent) :
	QWidget(parent), p(P),
	ui(new Ui::TDetail) {
	ui->setupUi(this);
    setWindowTitle(QString("编辑%1号节点").arg(P->v->id));
	ui->title->setText(QString::fromStdString(this->p->v->title));
	ui->content->setText(QString::fromStdString(this->p->v->text));
    move(cursor().pos());

	connect(ui->btn_save, &QPushButton::clicked, this, [ = ]() {
		p->v->title = ui->title->toPlainText().toStdString();
		p->v->text = ui->content->toPlainText().toStdString();
		p->setText(ui->title->toPlainText());
        p->refreshStyle(0);         // 2022/8/16 找了许久的bug 原本这行在析构函数 点击删除之后p被释放 但是窗口析构函数调用了p
		close();
	});
	connect(ui->btn_clear, &QPushButton::clicked, this, [ = ]() {
		ui->title->setText("");
		ui->content->setText("");
	});
	connect(ui->deleteP, &QPushButton::clicked, this, [ = ]() {
		((Graph*)(p->parent()))->removeP(p);
		close();
	});
	connect(ui->addE, &QPushButton::clicked, this, [ = ]() {
        TipLabel::showTip("点击要连的点",P->parent()->parent(),1000);    //会自动析构
	});
    connect(ui->editE, &QPushButton::clicked, this, [ = ]() {
        TipLabel::showTip("点击要编辑的边的另一端点",P->parent()->parent(),1000);
    });
	connect(ui->cutE, &QPushButton::clicked, this, [ = ]() {
        TipLabel::showTip("点击要删除的边的另一端点",P->parent()->parent(),1000);
	});
}

TDetail::~TDetail() {
	delete ui;
}
