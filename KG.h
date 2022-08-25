#ifndef KG_H_
#define KG_H_
#include <string>
#include <fstream>
#include "linklist.h"
#include <ctime>
using namespace std;

class Point;
struct E;
struct V;
struct Task;
class KG;

struct E {
	V* from;	//起点
	V* to;		//终点
	E* fnext;	//同起点的边
	E* tnext;	//同终点的边
	string text;//边的信息

	E(V* From = nullptr, V* To = nullptr, E* Fnext = nullptr, E* Tnext = nullptr, string Text = "") :
		from(From), to(To), fnext(Fnext), tnext(Tnext), text(Text) {}
	~E();
};

struct V {
	int id;
	string title;
	string text;
	E* e;
	linklist<Task*> task;
    Point* p;   // 绑定的ui

	V(int Id, string Title = "", string Text = "", E* in = nullptr, E* out = nullptr, Point* P = nullptr) :
		id(Id), title(Title), text(Text), p(P) {
		e = new E(this, this, in, out);
	}
	~V();

	/**
	 * @brief 查找有无到此节点的边
	 *
	 * @param ano 终点
	 *
	 * @return 找到则返回该边节点，没找到则返回nullprt
	 */
	E* Eto(V* ano);

	/**
	 * @brief 返回/新建 与此节点连接的边
	 *
	 * @param ano 终点
	 *
	 * @return 如果已经连接则返回已有的边，否则新建并返回
	 */
	E* to(V* ano);

	/**
	 * @brief 改变边在邻接表的位置
	 *
	 * @param InOrOut    true 表示出边, false表示入边
	 * @param moveWhere    移动到第几个(从0开始
	 * @param moveWho      移动哪条边
	 *
	 * @return 如果moveWho不在此节点的出边/入边中, 则返回false
	 */
	bool moveE(bool InOrOut, E* moveWho, int moveWhere);
};

struct Task {
	long id;		// 用时间戳定义id, 独一无二
	string text;
	linklist<V*> v;
	Task(string Text) :text(Text) {
		id = time(0);
	}
	~Task();
    bool bindV(V*);
	void unbindV(V*);
};

// 	KnowledgeGraph 类
class KG {
private:
	Node<V*>* Vtail;	//尾指针, 用于加速尾插和维护V的id
public:
	linklist<V*> v;
	linklist<Task*> task;

    KG(string path="") {
		Vtail = v.head;
        if(!path.empty()) KGread(this, path);
	}
	~KG() {	//不需要写任何内容, 析构由成员结构完成
		/*	KG析构时会自动:
			调用v表的析构:
				v表析构每个node
					每个node析构自己的V		...	V全部析构
						每个V析构相关联的E		...	E全部析构
			调用task表的析构:
				task表析构每个node
					每个node析构自己的Task	...	Task全部析构
		*/
	}

	// V的id指示的是其在链表的位置, 需要维护
	V* addV(string Title = "", string Text = "");
	V* getV(int index);			// 按id获取V
	bool removeV(int index);	// 按id删V
	bool removeV(V*);

	E* addE(int From, int To);
	E* getE(int From, int To);
	E* addE(V* From, V* To) = delete;	// 传参为节点指针的可以直接调用节点的to函数
	E* getE(V* From, V* To) = delete;	// 这两个被delete的函数都可以用节点的to函数替代, 因此可以不写
	void removeE(V* From, V* To);
	void removeE(int From, int To);

	Task* addTask(string Text);
	Task* getTask(long Id);		//按id找题
	void removeTask(long Id);	//按id删题

    void saveTo(string path);
    static void writestring(string p, ofstream& ofs);
    static string readstring(ifstream& ifs);
    static void KGwrite(KG*,string path);       // 把图谱数据存储到path
    static void KGread(KG*,string path);        // 从path读取图谱 要求kg已经new
};
#endif 
