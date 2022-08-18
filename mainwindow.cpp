#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow) {
	ui->setupUi(this);
    setAcceptDrops(true);
	resize(1200, 800);
	Vnum = new QLabel("节点个数:", this);
	ui->statusbar->addWidget(Vnum);
	graph = new Graph(this);
	setCentralWidget(graph);
	connect(ui->actionopen, &QAction::triggered, [ = ]() {
		qDebug() << "open";
	});
	connect(ui->actionnew, &QAction::triggered, []() {
		qDebug() << "new";
	});
	connect(ui->actionsave, &QAction::triggered, []() {
		qDebug() << "save";
	});
	connect(ui->actionsaveas, &QAction::triggered, []() {
		qDebug() << "saveas";
	});
	connect(ui->actionexport, &QAction::triggered, []() {
		qDebug() << "export";
	});
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::refreshStatus() {
	Vnum->setText(QString("节点个数%1").arg(graph->kg->v.length));
}
void MainWindow::dragEnterEvent(QDragEnterEvent *e){
    if(e->mimeData()->hasUrls()){
        e->accept();
    }
}
void MainWindow::dropEvent(QDropEvent* e){
    e->accept();
    qDebug()<<"mainwindow接收文件";
}
