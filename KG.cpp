#include "KG.h"
#include <sstream>
#include <QMessageBox>

E::~E()	{	// 维护十字链表
	if (from == to) return;		// 如果是头结点就不必维护
	// 出边
	E* t = this->from->e;
	for (; t->fnext != this; t = t->fnext);		// 找到前一个
	t->fnext = this->fnext;
	// 入边
	t = this->to->e;
	for (; t->tnext != this; t = t->tnext);
	t->tnext = this->tnext;
}


V::~V() {
	// 调用和V相关的所有E的析构
	for (; this->e->fnext; delete this->e->fnext);
	for (; this->e->tnext; delete this->e->tnext);
	delete e;

	// 从绑定的Task处断开关系
	Node<Task*>* q = task.head->next;
	while (q) {
		q->data->v.remove(this, false);
		q = q->next;
	}
	// 不析构task表中的指针, 因为只是关系并非存储
	task.clear();
}
E* V::Eto(V* ano) {
	// 遍历出边
	E* p = this->e->fnext;
	for (; p && p->to != ano; p = p->fnext);
	return p;
}
E* V::to(V* ano) {
	E* targetE = Eto(ano);
	// 查找是否已经连接
	if (targetE)	//如果已经连接则返回
		return targetE;
	//否则新建, e->fnext之后头插
	targetE = new E(this, ano, this->e->fnext, ano->e->tnext);
	this->e->fnext = targetE;
	ano->e->tnext = targetE;
	return targetE;
}
bool V::moveE(bool InOrOut, E* moveWho, int moveWhere) {
	E* t;
	int temp = moveWhere;
	if (InOrOut) {
		t = moveWho->from->e;
		for (; t && t->fnext != moveWho; t = t->fnext);
		if (!t) return false;
		t->fnext = moveWho->fnext;

		for (t = moveWho->from->e; temp > 0; temp--)
			t = t->fnext;
		moveWho->fnext = t->fnext;
		t->fnext = moveWho;
	} else {
		t = moveWho->to->e;
		for (; t && t->tnext != moveWho; t = t->tnext);
		if (!t) return false;
		t->tnext = moveWho->tnext;

		for (t = moveWho->to->e; temp > 0; temp--)
			t = t->tnext;
		moveWho->tnext = t->tnext;
		t->tnext = moveWho;
	}
	return true;
}

Task::~Task() {
	// 从绑定的V处断开关系
	Node<V*>* p = v.head->next;
	while (p) {
		p->data->task.remove(this, false);
		p = p->next;
	}
	// 不析构v表中的指针, 因为只是关系并非存储
	v.clear();
}
bool Task::bindV(V* tag) {
	if (v.indexOf(tag) == -1) {
		v.insert(0, tag);
		tag->task.insert(0, this);
		return true;
	} else return false;
}
void Task::unbindV(V* tag) {
	v.remove(tag, false);
	tag->task.remove(this, false);
}


V* KG::addV(string Title, string Text) {
	Vtail->next = new Node<V*>(new V(v.length++, Title, Text));
	Vtail = Vtail->next;
	return Vtail->data;
}
V* KG::getV(int index) {
	Node<V*>* p = v.to(index);
	if (p) return p->data;
	return nullptr;
}
V* KG::getV(string Title) {
	for (Node<V*>* p = v.head->next; p; p = p->next)
		if (p->data->title == Title) return p->data;
	return nullptr;
}
bool KG::removeV(int index) {
	if (index < 0) index += v.length;
	if (index >= v.length || index < 0) return false;

	Vtail = v.to(index - 1);			// 找到前一个
	Node<V*>* q = Vtail->next;
	int i = q->data->id;                // 2022/8/16 破大防 本来写在while前面 但是删第一个的时候Vtail为头结点 其V指针为空
	Vtail->next = q->next;
	v.length--;
	delete q;

	// 维护id和尾指针
	while (Vtail->next) {
		Vtail = Vtail->next;
		Vtail->data->id = i++;
	}
	return true;
}
bool KG::removeV(V* targetV) {
	Vtail = v.head;
	while (Vtail->next && Vtail->next->data != targetV) Vtail = Vtail->next;
	Node<V*>* p = Vtail->next;
	if (p) {
		Vtail->next = p->next;
		int i = p->data->id;
		delete p;
		v.length--;
		while (Vtail->next) {
			Vtail = Vtail->next;
			Vtail->data->id = i++;
		}
		return true;
	}
	return false;
}
E* KG::addE(int From, int To) {
	V* p = getV(From);
	V* q = getV(To);
	return p->to(q);
}
E* KG::getE(int From, int To) {
	V* p = getV(From);
	V* q = getV(To);
	return p->Eto(q);
}
void KG::removeE(V* From, V* To) {
	delete From->Eto(To);
}
void KG::removeE(int From, int To) {
	V* p = getV(From);
	V* q = getV(To);
	removeE(p, q);
}
Task* KG::addTask(string Text) {	// 位置对Task没有意义，所以头插
	return task.insert(0, new Task(Text))->data;
}

Task* KG::getTask(unsigned long Id)	{		//按id找题
	Node<Task*>* p = task.head->next;
	for (int i = 0; i < task.length; i++) {
		if (p->data->id == Id)
			return p->data;
		p = p->next;
	}
	return nullptr;
}
void KG::removeTask(unsigned long Id) {
	Node<Task*>* p = task.head;
	while (p->next && p->next->data->id != Id) p = p->next;
	Node<Task*>* q = p->next;
	if (q) {
		p->next = q->next;
		delete q;
		task.length--;
	}
}

void KG::saveTo(string path) {
	KGwrite(this, path);
}
void KG::exportTo(string path) {
	KGwrite(this, path, 2);
}

void KG::writestring(string p, ofstream& ofs, bool encryption) {
	char* c;
	int len = p.length() + 1;
	c = new char[len];
	for (int i = 0; i < (int)p.length(); i++) {
		c[i] = p[i];
		if (encryption)
			c[i] += i;
	}
	c[len - 1] = 0;
	ofs.write((char*)&len, sizeof(len));
	ofs.write((const char*)c, len);
	delete[]c;
}


string KG::readstring(ifstream& ifs, bool decrypt) {
	int len;
	char* tar;
	ifs.read((char*)&len, sizeof(int));
	tar = new char[len];
	ifs.read(tar, len);
	if (decrypt) {
		for (int i = 0; i < len - 1; i++)
			tar[i] -= i;
	}
	string t = tar;
	delete[]tar;
	return t;
}


void KG::KGwrite(KG* kg, string txt, char mode) {
	ofstream ofs;
	ofs.open(txt, ios::out | ios::binary);

	/*-----文件头-----*/
	writestring("KiloGraph", ofs);
	ofs.write(&mode, sizeof(char));
	bool encryption = (mode == '2');	// 如果是2则加密

	/*-----存节点-----*/
	int counter = kg->v.length;
	ofs.write((const char*)&counter, sizeof(counter));

	Node<V*>* p = kg->v.head->next;
	for (; p; p = p->next) {
		writestring(p->data->title, ofs, encryption);
		writestring(p->data->text, ofs, encryption);
	}

	/*------存边------*/
	string s = "a";
	for (Node<V*>* p = kg->v.head->next; p; p = p->next) {
		for (E* q = p->data->e->fnext; q; q = q->fnext) {
			writestring(s, ofs, encryption);        // 标识后面的是边 因为没有统计边个数故用此法
			ofs.write((const char*)&q->from->id, sizeof(int));
			ofs.write((char*)&q->to->id, sizeof(int));
			writestring(q->text, ofs, encryption);
		}
	}
	writestring(string("Eend"), ofs, encryption);

	/*------存题目------*/
	counter = kg->task.length;
	ofs.write((const char*)&counter, sizeof(int));
	for (Node<Task*>* p = kg->task.head->next; p; p = p->next) {
		// ID 题目文本 关联知识点数量 关联知识点id们
		ofs.write((const char*)&p->data->id, sizeof(p->data->id));
		writestring(p->data->text, ofs, encryption);
		ofs.write((const char*)&p->data->v.length, sizeof(int));
		for (Node<V*>* q = p->data->v.head->next; q; q = q->next)
			ofs.write((char*)&q->data->id, sizeof(int));
	}
	ofs.close();
}


char KG::KGread(KG* kg, string txt) {
	ifstream ifs;
	ifs.open(txt, ios::in | ios::binary);
	// 文件流打开
	if (!ifs.is_open()) return 0;
	// 文件头验证
	string title = readstring(ifs);
	if (title != "KiloGraph") {
		ifs.close();    // 文件头不对直接返回
		return 0;
	}
	// 文件模式
	char mode;
	ifs.read(&mode, sizeof(char));
	bool decrypt = (mode == '2');	// 模式2则解密

	string text;
	int counter, i;
	/*-------读点-------*/
	ifs.read((char*)&counter, sizeof(int));
	for (i = 0; i < counter; i++) {
		title = readstring(ifs, decrypt);
		text = readstring(ifs, decrypt);
		kg->addV(title, text);      // 写入是从头开始 读取是尾插
	}

	/*-------读边-------*/
	int from, to;
	V* startV = kg->getV(0);
	string sign = readstring(ifs, decrypt);
	while (sign != "Eend") {
		ifs.read((char*)&from, sizeof(int));
		ifs.read((char*)&to, sizeof(int));
		if (startV->id != from) startV = kg->getV(from);
		(startV->to(kg->getV(to)))->text = readstring(ifs, decrypt);
		sign = readstring(ifs, decrypt);
	}

	int counter1, n, vid;
	long TID;
	ifs.read((char*)&counter1, sizeof(int));
	for (i = 0; i < counter1; i++) {   //题目数量的循环
		ifs.read((char*)&TID, sizeof(long));
		Task* t = kg->addTask(readstring(ifs, decrypt));
		t->id = TID;
		ifs.read((char*)&counter, sizeof(int));
		for (n = 0; n < counter; n++) {
			ifs.read((char*)&vid, sizeof(int));
			t->bindV(kg->getV(vid));
		}
	}
	ifs.close();
	return mode;
}

void KG::taskwrite(KG *kg, string tarcsv) {
	ofstream ou(tarcsv);
	for (Node<Task*>*p = kg->task.head->next; p; p = p->next) {
		int pos = p->data->text.find(DIVIDE);
		ou << p->data->text.substr(0, pos) << ",";
		ou << p->data->text.substr(pos + DIVIDELENGTH) << ",";
		for (Node<V*>*q = p->data->v.head->next; q; q = q->next)
			ou << q->data->title << ",";
		ou << '\n';
	}
	ou.close();
}


void KG::taskread(KG* kg, string tarcsv) {
	ifstream incsv(tarcsv);
	string line;
	while (getline(incsv, line)) {
		istringstream sin(line);
		string str2, str;
		getline(sin, str, ',');
		getline(sin, str2, ',');
		str.append(DIVIDE);
		str.append(str2);
        Task* t = kg->addTask(str);
        while (getline(sin, str, ',')) {
            V* v = kg->getV(str);
            if(v) t->bindV(v);
        }
	}
	incsv.close();
}
