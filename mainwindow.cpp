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
    setStyleSheet("background:rgb(220,220,220);");
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

    QLabel* githubUrl = new QLabel("https://github.com/madderscientist/KiloGraph", this);
    githubUrl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->statusbar->addPermanentWidget(githubUrl);


    // 关闭tab
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [=](int i){
        Page* page=(Page*)(ui->tabWidget->widget(i));
        if(ui->tabWidget->tabText(i)[0] == '*'){
            QMessageBox::StandardButton result=QMessageBox::question(this, page->teacher?"图谱尚未保存":"学习进度尚未保存","是否仍然关闭？", QMessageBox::Yes|QMessageBox::No);
            if(result==QMessageBox::No) return;
        }
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
        // 两种模式保存的东西不一样
        if(page->teacher){
            if(page->dir.isEmpty()){
                saveAs();   // 另存为
            } else {
                Page* page=(Page*)ui->tabWidget->currentWidget();
                page->dir = page->dir;
                page->graph->kg->saveTo(page->dir.toStdString());
                saved();    // 星号取消
            }
        } else {
            if(!page->student) TipLabel::showTip("还未导入进度文件！", this);
            else page->student->save(),saved();
        }
	});
    // 另存为
    ui->actionsaveas->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(ui->actionsaveas, &QAction::triggered, this, &MainWindow::saveAs);
    // 导出
    ui->actionexport->setShortcut(QKeySequence("Ctrl+E"));
    connect(ui->actionexport, &QAction::triggered, this, [ = ]() {
        QString fileName = QFileDialog::getSaveFileName(this, "导出图谱", "/stuKG.skg", QStringLiteral("学生用图谱(*.skg)"));
        if (!fileName.isNull()) {
            Page* page=(Page*)ui->tabWidget->currentWidget();
            page->graph->kg->exportTo(fileName.toStdString());
            TipLabel::showTip("导出成功", this);
        } else TipLabel::showTip("导出取消", this);
	});
    // 导入(进度文件)
    ui->actionimport->setShortcut(QKeySequence("Ctrl+I"));
    connect(ui->actionimport, &QAction::triggered, this, [ = ]() {
        Page* page=(Page*)ui->tabWidget->currentWidget();
        bool ifcover = false;
        // menuBan函数决定了只有teacher=false的界面可以使用此菜单
        if(page->student) {
            QMessageBox::StandardButton result=QMessageBox::question(this, "已导入进度","是否更换进度文件？", QMessageBox::Yes|QMessageBox::No);
            if(result==QMessageBox::No) return;
            else ifcover = true;
        }
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选择进度文件"),"/",QStringLiteral("学习进度(*.learn)"));
        if(!fileName.isEmpty()) {
            if(ifcover) delete page->student;
            page->student = new Student(page, fileName);
        }
    });
    // 新建进度文件
    ui->actionnewlearn->setShortcut(QKeySequence("Ctrl+L"));
    connect(ui->actionnewlearn, &QAction::triggered, this, [ = ]() {
        Page* page=(Page*)ui->tabWidget->currentWidget();
        bool ifcover = false;
        // menuBan函数决定了只有teacher=false的界面可以使用此菜单
        if(page->student) {
            QMessageBox::StandardButton result=QMessageBox::question(this, "已导入进度","是否更换进度文件？", QMessageBox::Yes|QMessageBox::No);
            if(result==QMessageBox::No) return;
            else ifcover = true;
        }
        if(ifcover) delete page->student;
        page->student = new Student(page, "");
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

// teacher模式下保存图片文件 student模式下保存进度文件
void MainWindow::saveAs(){
    Page* page=(Page*)ui->tabWidget->currentWidget();
    if(page->teacher){
        QString fileName = QFileDialog::getSaveFileName(this, "保存图谱", "/newKG.kg", QStringLiteral("图谱(*.kg)"));
        if (!fileName.isNull()) {
            page->dir = fileName;
            page->graph->kg->saveTo(fileName.toStdString());
            ui->tabWidget->setTabText(tabIndex,fileName.mid(fileName.lastIndexOf('/')+1));
            saved();
        }
    } else {
        if(!page->student) TipLabel::showTip("还未导入进度文件！", this);
        else page->student->save(true);
    }
}

void MainWindow::newPage(QString path){
    Page* p=new Page(this, path);
    QTimer::singleShot(0, this, [ = ]() {   // 必须延时 执行顺序: 执行完本函数，graph的延时0秒，本函数的延时0秒
        // 我猜计时器是和parent挂钩的 所以不延时的时候tabWidget先给我偷换parent，然后graph的timer就不能从本来的parent访问graph了
        QString simpleName = "*新建";
        if(!path.isEmpty()) {
            int x = path.lastIndexOf('/');
            int y = path.lastIndexOf('\\');
            simpleName = path.mid((x>y?x:y)+1);
        }
        ui->tabWidget->addTab(p, simpleName);
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        // 内容有更改则tab加*
        connect(p, &Page::ContentChanged, this, [=](){
            int I=ui->tabWidget->indexOf((QWidget*)p);
            if(I>-1){
                QString x=ui->tabWidget->tabText(I);
                if(x[0]!='*') ui->tabWidget->setTabText(I, "*"+x);
            }
        });
        connect(p, &Page::renameTab, this, [=](QString name){
            int I=ui->tabWidget->indexOf((QWidget*)p);
            if(I>-1) ui->tabWidget->setTabText(I, name);
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
    QString path = e->mimeData()->urls()[0].toLocalFile();
    if(path.right(3)=="arn"){
        if(ui->tabWidget->count()){
            Page* page=(Page*)ui->tabWidget->currentWidget();
            if(!page->teacher){
                if(page->student){
                    QMessageBox::StandardButton result=QMessageBox::question(this, "已导入进度","是否更换进度文件？", QMessageBox::Yes|QMessageBox::No);
                    if(result==QMessageBox::No) return;
                    else delete page->student;
                }
                page->student = new Student(page, path);
                e->accept();
                return;
            }
        }
        TipLabel::showTip("请先打开.skg文件！",this);
    }
    else newPage(path);
    e->accept();
}
void MainWindow::menuBan(){
    // 不受限制的菜单: 新建 打开
    // 通用菜单: 保存 另存为
    bool show = ui->tabWidget->count();
    ui->actionsave->setEnabled(show);
    ui->actionsaveas->setEnabled(show);
    if(show){
        // teacher模式菜单: 导出
        // student模式菜单: 导入 新建学习进度
        show = ((Page*)ui->tabWidget->currentWidget())->teacher;
        ui->actionimport->setEnabled(!show);
        ui->actionnewlearn->setEnabled(!show);
        ui->actionexport->setEnabled(show);
    } else {
        ui->actionimport->setEnabled(false);
        ui->actionnewlearn->setEnabled(false);
        ui->actionexport->setEnabled(false);
    }
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

void MainWindow::closeEvent(QCloseEvent *e){
    // 若有文件没保存则提示关闭
    for(int i=0;i<ui->tabWidget->count();i++){
        if(ui->tabWidget->tabText(i)[0] == '*')
            if(QMessageBox::No == QMessageBox::question(this, "有文件尚未保存","是否仍然关闭？", QMessageBox::Yes|QMessageBox::No)){
                e->ignore();
                return;
            }
    }
    e->accept();
}
