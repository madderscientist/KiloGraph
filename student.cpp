#include "student.h"
#include "page.h"
#include <QFileDialog>

StuProgress::StuProgress() {
	stu = new map<long, string>;
}
StuProgress::~StuProgress() {
	delete stu;
}
string& StuProgress::id(long id) {
	return (*stu)[id];
}
bool StuProgress::SPread(StuProgress* sp, string path) {
	ifstream ifs;
	ifs.open(path, ios::in | ios::binary);
	// 文件流打开
	if (!ifs.is_open()) return false;
	// 文件头验证
	string title = KG::readstring(ifs);
	if (title != "StuProgress") {
		ifs.close();    // 文件头不对直接返回
		return false;
	}
	int size;
	ifs.read((char*)&size, sizeof(size));
	long ID;
	for (int i = 0; i < size; i++) {
		ifs.read((char*)&ID, sizeof(ID));
		sp->id(ID) = KG::readstring(ifs);
	}
	ifs.close();
	return true;
}
bool StuProgress::SPwrite(StuProgress* sp, string path) {
	ofstream ofs;
	ofs.open(path, ios::out | ios::binary);
	if (!ofs.is_open()) return false;

	KG::writestring("StuProgress", ofs);
	int size = sp->stu->size();
	ofs.write((char*)&size, sizeof(size));
	for (auto it = sp->stu->begin(); it != sp->stu->end(); it++) {
		ofs.write((char*)&it->first, sizeof(long));
		KG::writestring(it->second, ofs);
	}
	ofs.close();
	return true;
}




Student::Student(Page* PAGE, QString path):
	page(PAGE), dir(path) {
	sp = new StuProgress();
	QTimer::singleShot(0, PAGE, [ = ]() {
		QString tabtitle;
		if (!path.isEmpty()) {
			if (StuProgress::SPread(sp, path.toStdString()))
				TipLabel::showTip("进度读取成功！", page);
			else {
				TipLabel::showTip("进度读取失败！", page);
				page->student = nullptr;
			}
			tabtitle = path.mid(path.lastIndexOf('/') + 1);
		} else {
			TipLabel::showTip("新建进度文件成功！", page);
			tabtitle = "*新进度";
		}
		if (page->student) {
			iniMastery();
			emit PAGE->renameTab(tabtitle);
		} else delete this;
	});
}
Student::~Student() {
	page->student = nullptr;
	delete[]mastery;
	delete dotask;
	delete sp;
}
void Student::save(bool saveAs) {
	if (saveAs) goto SaveAs;
	if (dir.isEmpty()) {
SaveAs:
		QString fileName = QFileDialog::getSaveFileName(page, "保存学习进度", "/progress.learn", QStringLiteral("学习进度(*.learn)"));
		if (!fileName.isNull())
			dir = fileName;
		else {
			TipLabel::showTip("保存取消！", page);
			return;
		}
	}
	TipLabel::showTip(StuProgress::SPwrite(sp, dir.toStdString()) ? "进度保存成功！" : "进度保存失败！", page);
}
void Student::exercise(Point* p) {
	delete dotask;
	dotask = new DoTask(p, page);
}
void Student::refreshMastery() {
	// 对所有知识点
	for (int i = 0; i < page->graph->kg->v.length; i++) {
		mastery[i].doneNum = 0;
		mastery[i].rightNum = 0;
		// 对一个知识点下的所有题目
		for (int j = 0; j < mastery[i].taskNum; j++) {
			string &str = sp->id(mastery[i].tasks[j]->id);
			if (str.length()) {
				mastery[i].doneNum++;
				if (str[str.length() - 1] == '1') mastery[i].rightNum++;
			}
		}
	}
}
void Student::iniMastery() {
	int length = page->graph->kg->v.length;
	mastery = new Mastery[length];
	bool* notvisit = new bool[length];
	queue<V*> q;
	linklist<Task*> tasklist;
	Node<V*>* n = page->graph->kg->v.head->next;

	while (n) {     // 对每个节点
		for (int i = 0; i < length; i++) notvisit[i] = true;
		q.push(n->data);
		notvisit[n->data->id] = false;
		while (!q.empty()) {
			V* v = q.front();
			Node<Task*>* t = v->task.head->next;
			while (t) {
				tasklist.setPush(t->data);
				t = t->next;
			}
			E* e = v->e->fnext;
			while (e) {
				if (notvisit[e->to->id]) {
					q.push(e->to);
					notvisit[e->to->id] = false;
				}
				e = e->fnext;
			}
			q.pop();
		}
		Mastery& m = mastery[n->data->id];
		m.taskNum = tasklist.length;
		m.tasks = tasklist.toArray();
		tasklist.clear();
		for (int i = 0; i < m.taskNum; i++) {
			string &str = sp->id(m.tasks[i]->id);
			if (str.length()) {
				m.doneNum++;
				if (str[str.length() - 1] == '1') m.rightNum++;
			}
		}
		n = n->next;
	}
	delete[]notvisit;
}
