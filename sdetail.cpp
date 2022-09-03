#include "sdetail.h"
#include "page.h"
#include <QMessageBox>
#include "student.h"

SDetail::SDetail(Point* p, Page *parent)
	: Card(QString::number(p->v->id) + "号节点详情", parent), P(p) {
	connect(Card::top->off, &QPushButton::clicked, this, [ = ]() {
		p->detail = nullptr;
	});

	QLabel* title = new QLabel(p->v->title.empty() ? "无标题" : QString::fromStdString(p->v->title), this);
	title->setStyleSheet("background:transparent;font-size:30px;margin:0px;padding:0px;");

	QPlainTextEdit* text = new QPlainTextEdit(QString::fromStdString(p->v->text), this);
	text->setReadOnly(true);
	text->setStyleSheet("background:transparent;font-size:23px;border:2px solid red;border-radius:16px;padding:5px 8px;");


	QPushButton* exercise = new QPushButton("开始练习", this);
	Card::niceButton(exercise);

	Card::Layout->addWidget(title, 0);
	Card::Layout->addWidget(text, 1);
	if (parent->student) {
		statistics = new QLabel(this);
		statistics->setStyleSheet("background:transparent;font-size:23px;border:2px solid red;border-radius:16px;padding:5px 8px;");
		Card::Layout->addWidget(statistics);
		Statistic();
	}
	Card::Layout->addWidget(exercise, 0);

	resize(CardW - 10, CardW - 10);

	// 开始做题 没文件则新建进度文件 有文件则直接开始
	connect(exercise, &QPushButton::clicked, parent, [ = ]() {
		if (parent->student) {
			parent->student->exercise(p);
			p->detail = nullptr;
			this->close();
		} else {
			if (QMessageBox::question(this, "还没有开始学习", "是否新建进度文件？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
				parent->student = new Student(parent, "");
				QTimer::singleShot(1, this, [ = ]() {
					parent->student->exercise(p);
					p->detail = nullptr;
					this->close();
				});
			}
		}
	});
}
// 统计完成情况
void SDetail::Statistic() {
	Mastery &m = ((Page*)parent())->student->mastery[P->v->id];
	statistics->setText(QString("总题数:%1\n已做题数:%2\n正确率:%3\n")
	                    .arg(m.taskNum)
	                    .arg(m.doneNum)
	                    .arg(m.doneNum ? QString::number(m.rightNum * 100.0 / m.doneNum) + "%" : "0%"));
}


DoTask::DoTask(Point* p, Page *parent):
	Card(QString::fromStdString(p->v->text + "练习"), parent), v(p->v) {
	connect(Card::top->off, &QPushButton::clicked, parent, [ = ]() {
		parent->student->dotask = nullptr;
	});

	taskText = new QPlainTextEdit(this);
	taskText->setStyleSheet("QPlainTextEdit{font-size:20px;padding:5px;background:rgba(255,255,255,0.8);border-radius:15px;}");
	taskText->setReadOnly(true);

	ans = new QLineEdit(this);
	Card::niceLineEdit(ans);

	judge = new QLabel(this);   // 占位 同时显示答案正确与否
	judge->setStyleSheet("background:transparent;font-size:20px;");

	QPushButton* next = new QPushButton("确定", this);
	Card::niceButton(next);
	next->setMinimumWidth(100);

	QHBoxLayout* hbl = new QHBoxLayout;
	hbl->addWidget(judge, 1);
	hbl->addWidget(next, 0);

	Card::Layout->addWidget(taskText);
	Card::Layout->addWidget(ans);
	Card::Layout->addLayout(hbl);

	resize(1.5 * CardW, 1.5 * CardW * 0.618);
	move(parent->width() / 2 - CardW, 20);
	show();
	if (iniData()) {
		TipLabel::showTip("没有题目", parent);
		QTimer::singleShot(0, this, &QWidget::close);
		return;
	}

	pickTask();
	showTask();

	connect(next, &QPushButton::clicked, this, [ = ]() {
		if (next->text() == "下一题") {
			judge->clear();
			ans->clear();
            ans->setFocus();
			pickTask();
			showTask();
			next->setText("确定");
		} else {
			if (ans->text().isEmpty())
				TipLabel::showTip("请输入答案！", parent, 600);
			else {
				// 判断是否正确
				QString x(QString::fromStdString(currentTask->text));
				int i = x.indexOf(DIVIDE);
                QString y = x.mid(i + DIVIDELENGTH);
                QString answer = ans->text().remove(QRegExp("\\s")).toUpper();
                if (answer == y || answer == y.toUpper()) {
					judge->setText("回答正确");
					updateWeight(1);
				} else {
					judge->setText(QString("回答错误 答案是:%1").arg(y));
					updateWeight(0);
				}
				next->setText("下一题");
				emit parent->ContentChanged();
			}
		}
	});
}

DoTask::~DoTask() {
	delete[]weight;
}

bool DoTask::iniData() {
	Mastery &m = ((Page*)parent())->student->mastery[v->id];
	if (!m.taskNum)return 1;
	weight = new float[m.taskNum];
	for (int i = 0; i < m.taskNum; i++) {
		weight[i] = 0;
		string &str = ((Page*)parent())->student->sp->id(m.tasks[i]->id);
		if (str.empty()) {
			weight[i] = 8;
			continue;
		}
		for (int j = 0; j < (int)str.length(); j++)
			weight[i] += 1 / (str.length() - j) * ('1' - str[j]);
	}
	return 0;
}

void DoTask::showTask() {
	QString str = QString::fromStdString(currentTask->text);
	str = str.mid(0, str.indexOf(DIVIDE));
	taskText->setPlainText(str);
}

void DoTask::pickTask() {
	// 找权值"最大"的题出
	Mastery &m = ((Page*)parent())->student->mastery[v->id];
	float k = -10;
	int z = 0;
	for (int i = 0; i < m.taskNum; i++) {
		if (weight[i] > k) k = weight[i], z = i;
	}
	if (k < 1) {
		currentTask = m.tasks[z];
		TipLabel::showTip("已经全部掌握啦", parent());
	} else {
		// 随机选择
		Task** maxs = new Task*[m.taskNum];
		int maxnum = 0;
		for (int i = 0; i < m.taskNum; i++)
			if (k - weight[i] < 2) maxs[maxnum++] = m.tasks[i];
		srand(time(NULL));

		// 不重复
		Task* pic = maxs[rand() % maxnum];
		while (pic == currentTask && maxnum > 1)
			pic = maxs[rand() % maxnum];
		currentTask = pic;
		delete[]maxs;
	}
}

void DoTask::updateWeight(bool a) {
	Mastery &m = ((Page*)parent())->student->mastery[v->id];

	int i = 0;
	for (; i < m.taskNum && m.tasks[i] != currentTask; i++);

	string& str = ((Page*)parent())->student->sp->id(currentTask->id);

	if (a) {
		weight[i] = -0.2;
		str += '1';
	} else {
		weight[i] = 1;
		str += '0';
	};
	for (int j = 0; j < (int)str.length(); j++) weight[i] += ('1' - str[j]) / (str.length() - j);

	// 把关联的知识点全部更新
	((Page*)parent())->student->refreshMastery();
}
