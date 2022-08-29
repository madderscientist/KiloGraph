#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow) {
	ui->setupUi(this);
    setWindowIcon(QIcon(":/logo.ico"));
    setAcceptDrops(true);
    setCentralWidget(ui->tabWidget);
    setStyleSheet("background:rgb(219,219,219);");
    menuBan();

    // 状态栏
    Pause = new QPushButton("||",this);
    ui->statusbar->addWidget(Pause);

    Vnum = new QLabel("节点个数:0", this);
    ui->statusbar->addWidget(Vnum);
    Vnum->setFixedWidth(Vnum->width()+80);

    Damping=new QLabel("阻尼:1.2", this);
    ui->statusbar->addWidget(Damping);
    Damping->setFixedWidth(Damping->width()+80);

    DampingSlider=new QSlider(Qt::Horizontal,this);
    DampingSlider->setMaximum(100);
    DampingSlider->setMinimum(0);
    ui->statusbar->addWidget(DampingSlider);


    // 关闭tab
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [=](int i){
        if(ui->tabWidget->tabText(i)[0]=='*'){
            QMessageBox::StandardButton result=QMessageBox::question(this, "图谱尚未保存","是否仍然关闭？", QMessageBox::Yes|QMessageBox::No);
            if(result==QMessageBox::No) return;
        }
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
            refreshStatus(temp->graph->kg->v.length);
        }
        menuBan();
        pageSettings();
    });
    // 打开
    ui->actionopen->setShortcut(QKeySequence("Ctrl+O"));
    connect(ui->actionopen, &QAction::triggered, this, [ = ]() {
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选择图谱文件"),"/",QStringLiteral("图谱(*kg *skg)"));
        if(!fileName.isEmpty()) newPage(fileName);
    });
    // 新建
    ui->actionnew->setShortcut(QKeySequence("Ctrl+N"));
    connect(ui->actionnew, &QAction::triggered, [=](){newPage();});
    // 保存
    ui->actionsave->setShortcut(QKeySequence("Ctrl+S"));
    connect(ui->actionsave, &QAction::triggered, [ = ]() {
        Page* page = (Page*)(ui->tabWidget->currentWidget());
        if(page->dir.isEmpty()){
            saveAs();   // 另存为
        } else {
            Page* page=(Page*)ui->tabWidget->currentWidget();
            page->dir = page->dir;
            page->graph->kg->saveTo(page->dir.toStdString());
            saved();    // 星号取消
        }
	});
    // 另存为
    ui->actionsaveas->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(ui->actionsaveas, &QAction::triggered, this, &MainWindow::saveAs);
    // 导出
    ui->actionexport->setShortcut(QKeySequence("Ctrl+E"));
    connect(ui->actionexport, &QAction::triggered, this, [ = ]() {
        QString fileName = QFileDialog::getSaveFileName(this, "导出图谱", "/", QStringLiteral("学生用图谱(*skg)"));
        qDebug()<<fileName;
        if (!fileName.isNull()) {
            //fileName是文件名
        } else {
            //点的是取消
        }
	});
    // theworld！
    connect(Pause, &QPushButton::clicked, this, [=](){
        if(tabIndex>-1&&tabIndex<ui->tabWidget->count()){
            Page* page = (Page*)ui->tabWidget->currentWidget();
            page->physics=!page->physics;
            Pause->setText(page->physics?"||":"▶");
        }
    });
    // 滑块
    connect(DampingSlider, &QSlider::valueChanged, this, [=](int pos){
        if(tabIndex>-1&&tabIndex<ui->tabWidget->count()){
            Page* page = (Page*)ui->tabWidget->currentWidget();
            float newDamp=toDamp(pos);
            Damping->setText(QString("阻尼: %1").arg(newDamp));
            page->graph->DAMPING = newDamp;
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this, "保存图谱", "/newKG.kg", QStringLiteral("图谱(*.kg)"));
    if (!fileName.isNull()) {
        Page* page=(Page*)ui->tabWidget->currentWidget();
        page->dir = fileName;
        page->graph->kg->saveTo(fileName.toStdString());
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), fileName.mid(fileName.lastIndexOf('/')+1));
        saved();
    }
}

void MainWindow::newPage(QString path){
    Page* p=new Page(this, path);
    QTimer::singleShot(0, this, [ = ]() {   // 必须延时 执行顺序: 执行完本函数，graph的延时0秒，本函数的延时0秒
        // 我猜计时器是和parent挂钩的 所以不延时的时候tabWidget先给我偷换parent，然后graph的timer就不能从本来的parent访问graph了
        ui->tabWidget->addTab(p, path.isEmpty()?"*新建":path.mid(path.lastIndexOf('/')+1));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        // 内容有更改则tab加*
        connect(p, &Page::ContentChanged, ui->tabWidget, [=](){
            int I=ui->tabWidget->indexOf((QWidget*)p);
            if(I>-1){
                QString x=ui->tabWidget->tabText(I);
                if(x[0]!='*') ui->tabWidget->setTabText(I, "*"+x);
            }
        });
    });
}

void MainWindow::refreshStatus(int i) {
    Vnum->setText(QString("节点个数:%1").arg(i));
}
void MainWindow::dragEnterEvent(QDragEnterEvent *e){
    if(e->mimeData()->hasUrls()){
        e->accept();
    }
}
void MainWindow::dropEvent(QDropEvent* e){
    newPage(e->mimeData()->urls()[0].toLocalFile());
    e->accept();
}
void MainWindow::menuBan(){
    bool show = ui->tabWidget->count();
    ui->actionsave->setEnabled(show);
    ui->actionsaveas->setEnabled(show);
    ui->actionexport->setEnabled(show);
}

// 阻尼和暂停的初始化
void MainWindow::pageSettings(){
    if(tabIndex>-1&&tabIndex<ui->tabWidget->count()){
        Page* page = (Page*)ui->tabWidget->currentWidget();
        if(page){
            Damping->setText(QString("阻尼:%1").arg(page->graph->DAMPING));
            DampingSlider->setSliderPosition(toSlider(page->graph->DAMPING));
            Pause->setText(page->physics?"||":"▶");
        }
    }
}

// 阻尼范围 0.8~20.2 太小会过分Q弹 大于21会导致速度反向(因为时间是离散的) 分段函数
int MainWindow::toSlider(float i){
    if(i>6.1) return (9.8+i)/0.3;
    else return 10*i-8;
}
float MainWindow::toDamp(int i){
    if(i>53) return 0.3*i-9.8;
    else return 0.8+0.1*i;
}
// 星号消除
void MainWindow::saved(){
    QString x=ui->tabWidget->tabText(tabIndex);
    if(x[0]=='*') ui->tabWidget->setTabText(tabIndex, x.mid(1));
}
