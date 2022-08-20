#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow) {
	ui->setupUi(this);
    setAcceptDrops(true);
    setCentralWidget(ui->tabWidget);
    setStyleSheet("background:rgb(219,219,219);");
    menuBan();


    Vnum = new QLabel("节点个数:", this);
    ui->statusbar->addWidget(Vnum);


    // 关闭tab
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [=](int i){
        Page* page=(Page*)(ui->tabWidget->widget(i));
        ui->tabWidget->removeTab(i);
        delete page;
    });
    // tab更改
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int i){
        if(tabIndex>=0 && tabIndex<ui->tabWidget->count())  // 之前没有加后一个条件 导致关闭界面时崩溃
            ((Page*)(ui->tabWidget->widget(tabIndex)))->graph->timerSwitch(false);
        tabIndex=i;
        if(tabIndex>=0){
            Page* temp=(Page*)(ui->tabWidget->currentWidget());
            temp->graph->timerSwitch(true);
            temp->pointNumChange();
        }
        menuBan();
    });
    // 打开
    connect(ui->actionopen, &QAction::triggered, this, [ = ]() {
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选择图谱文件"),"/",QStringLiteral("图谱(*kg *skg)"));
        qDebug()<<fileName;
    });
    // 新建
    connect(ui->actionnew, &QAction::triggered, this, [ = ]() {
        Page* p=new Page(this);
        QTimer::singleShot(0, this, [ = ]() {   // 必须延时 执行顺序: 执行完本函数，graph的延时0秒，本函数的延时0秒
            // 我猜计时器是和parent挂钩的 所以不延时的时候tabWidget先给我偷换parent，然后graph的timer就不能从本来的parent访问graph了
            ui->tabWidget->addTab(p,"新建");
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        });
	});
    // 保存
    connect(ui->actionsave, &QAction::triggered, [ = ]() {
        Page* page = (Page*)(ui->tabWidget->currentWidget());
        if(page->dir.isNull()){
            saveAs();   // 另存为
        } else {
            // 保存
        }
	});
    // 另存为
    connect(ui->actionsaveas, &QAction::triggered, this, &MainWindow::saveAs);
    // 导出
    connect(ui->actionexport, &QAction::triggered, this, [ = ]() {
        QString fileName = QFileDialog::getSaveFileName(this, "导出图谱", "/", QStringLiteral("学生用图谱(*skg)"));
        qDebug()<<fileName;
        if (!fileName.isNull()) {
            //fileName是文件名
        } else {
            //点的是取消
        }
	});
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this, "保存图谱", "/", QStringLiteral("图谱(*kg)"));
    qDebug()<<fileName;
    if (!fileName.isNull()) {
        //fileName是文件名
    } else {
        //点的是取消
    }
}

void MainWindow::refreshStatus(int i) {
    Vnum->setText(QString("节点个数: %1").arg(i));
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
void MainWindow::menuBan(){
    bool show = ui->tabWidget->count();
    ui->actionsave->setEnabled(show);
    ui->actionsaveas->setEnabled(show);
    ui->actionexport->setEnabled(show);
}
