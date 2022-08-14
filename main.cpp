#include "KG.h"
#include<iostream>
using namespace std;
void print(KG& g){
	g.v.forEach([](Node<V*>* n, int i) {
		cout << "第" << i <<n->data->id<< "个知识点：" << n->data->title << ", " << n->data->text << '\n';
		cout << "\t相关题目: \n";
		n->data->task.forEach([](Node<Task*>* m, int j) {
			cout << "\t\t" << m->data->text << '\n';
		});
		cout << "\n\t相关知识点: \n";
		E* p = n->data->e->fnext;
		while (p) {
			cout << "\t\t" << p->text << '\t' << p->to->title << '\t' << p->to->text << '\n';
			p = p->fnext;
		}
		p = n->data->e->tnext;
		while (p) {
			cout << "\t\t" << p->text << '\t' << p->from->title << '\t' << p->from->text << '\n';
			p = p->tnext;
		}
		cout << "\n\n";
	});
}
int main() {
	KG kg;
	kg.addV("高中生物", "知识图谱");
	V* x=kg.addV("分子与细胞", "第一本");
	V* xx=kg.addV("遗传与进化", "第二本");
	kg.addE(0, 1)->text="分册";
	E* y = kg.addE(0, 2);
	y->text = "分册";
	y->from->moveE(true, y, 1);
	kg.addE(1, 2)->text="先后学习";
	kg.addTask("第一本书的习题册")->bindV(x);
	kg.addTask("第一本书的期末考试")->bindV(x);
	kg.addTask("第二本书的习题册")->bindV(xx);

	Task* t = kg.addTask("疫情取消的第一本书的期中考试");
	t->bindV(x);
	print(kg);
	cout << "删除被取消的考试:\n";
	kg.removeTask(t->id);
	print(kg);
	cout << "删除第一本书:\n";
	kg.removeV(x);
	print(kg);
	return 0;
}