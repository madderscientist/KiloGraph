/*
头结点 单链表 模板类
！模板只能传指针! 析构会释放存入的指针！
构造：(项数,填充)（带缺省值的全参构造）

[]重载：负数代表倒数,超过现有长度则添加,返回节点指针
to(位置)：返回节点指针
（推荐使用to函数）

insert(位置，值)：位置从零开始,不支持负数与超过
(注：头插最快，尾插最慢，如需尾插请自己添加尾节点)
setPush(值)：不重复尾插
remove(位置)：位置从零开始，不支持超过,支持负数

indexOf(值,从第几个开始找（默认从头）)：返回位置,没找到则返回-1
indexOf(Node*)：返回节点的位置

toArray()：链表转数组
cut(开始节点)：删除开始节点及其后面所有数据
clear()：不析构内容的清空，留下头结点
*/
#ifndef LINKLIST_H_
#define LINKLIST_H_
template <class T>
struct Node {	// 链表节点. 注意只能传指针类型！
	T data;
	Node<T>* next;

	/**
	* @brief 带全部缺省值的 全参构造函数
	*
	* @param d data
	* @param n next
	*/
	Node(T d = nullptr, Node<T>* n = nullptr) : data(d), next(n) {}

	/**
	* @brief 析构函数, 会析构成员变量data!!!
	*/
	~Node() {
		delete data;
	}

	/**
	* @brief 从该节点往后找n+1个(n从0开始)
	*
	* @param n
	*
	* @return 要访问的节点的指针,越界则返回nullptr
	*/
	Node<T>* to(int n) {
		Node<T>* p = this;
		for (int i = 0; i <= n && p != nullptr; i++)
			p = p->next;
		return p;
	}
};

template <class T>
class linklist {
	// 注意，最后一个指针的next始终要维护为nullptr
	// 只有析构才会delete头结点
public:
	int length;
	Node<T>* head;

	/**
	* @brief 带全部缺省值的 全参构造函数
	*
	* @param n 初始链表长度
	* @param value 填充值
	*/
	linklist(int n = 0, T value = nullptr) : length(n), head(nullptr) {
		for (int i = 0; i < n; i++)
			head = new Node<T>(value, head);
		head = new Node<T>(nullptr, head);
	}

	/**
	* @brief 析构函数, 将每一项及其data析构
	*/
	~linklist() {
		cut(head);	// 析构即：从头结点开始cut
	}

	/**
	 * @brief 清空链表, 但不析构data
	 * 若析构时不想析构指针内容请在delete前使用此函数(delete会把头结点释放)
	 */
	void clear() {
		cut(head->next, false);
	}

	/**
	* @brief 删除节点start及其之后的所有节点
	*
	* @param ifdele 是否析构节点的data, 默认会析构
	* @param start 删除的第一个节点
	*
	* @return 如果start不在表中则返回false
	*/
	bool cut(Node<T>* start, bool ifdele = true) {
		Node<T>* p;
		// 找到start, 维护最后一个节点的next为nullptr
		if (start != head) {
			p = head;
			while (p && p->next != start) {
				p = p->next;
			}
			if (!p) return false;	// 没找到start
			p->next = nullptr;
		}
		// 开始删除
		while (start != nullptr) {
			p = start;
			start = start->next;
			if (!ifdele) p->data = nullptr;
			delete p;
			length--;
		}
		return true;
	}


	/**
	* @brief 按index访问节点
	*
	* @param x 要访问第几项
	*
	* @return 要访问的节点的指针, 若x不在[0,length)内则返回nullptr
	*/
	Node<T>* to(int x) {
		return head->to(x);
	}

	/**
	* @brief 按index访问节点, 一定会返回节点指针
	*
	* @param x 要访问第几项 (x可以为任意整数, x为负数则倒数, x大于length则填充中间项)
	*
	* @return 要访问的节点的指针
	*/
	Node<T>* operator[](int x) {
		while (x < 0)
			x += length;
		Node<T>* p = to(x);
		for (; length < x + 1; length++) {
			p->next = new Node<T>;
			p = p->next;
		}
		return p;
	}

	/**
	* @brief 按index插入一项
	*
	* @param value 掺入节点的data
	* @param x 插入节点的位置
	*
	* @return 插入的节点的指针
	*/
	Node<T>* insert(int x, T value = nullptr) {
		if (x > length || x < 0)
			return nullptr;
		Node<T>* p = to(x - 1);
		p->next = new Node<T>(value, p->next);
		length++;
		return p->next;
	}

    /**
    * @brief 集合插入 元素不重复
    *
    * @param value 掺入节点的data
    *
    * @return 插入的节点的指针 如果重复则返回空指针
    */
    Node<T>* setPush(T value = nullptr) {
        Node<T>* p = head;
        while(p->next){
            if(p->next->data == value) return nullptr;
            p=p->next;
        }
        p->next = new Node<T>(value, nullptr);
        length++;
        return p->next;
    }

	/**
	* @brief 按index删除节点
	*
	* @param ifdele 是否析构节点的data, 默认会析构
	* @param x 删除第x个(x从零开始, 负数表示倒数)
	*
	* @return 删除成功返回true, x范围错误返回false
	*/
	bool remove(int x, bool ifdele = true) {
		if (x < 0) x += length;
		if (x >= length || x < 0) return false;

		Node<T>* p = to(x - 1);	// 找到前一个节点
		Node<T>* q = p->next;

		p->next = q->next;
		length--;

		if (!ifdele) q->data = nullptr;	//如果不析构则设为nullptr
		delete q;
		return true;
	}

	/**
	* @brief 按data删除节点
	*
	* @param ifdele 是否析构节点的data, 默认会析构
	* @param data 要删除节点的值
	*
	*/
	void remove(T data, bool ifdele = true) {
		Node<T>* p = head;
		while (p->next && p->next->data != data) p = p->next;
		Node<T>* q = p->next;
		if (q) {
			p->next = q->next;
			if (!ifdele) q->data = nullptr;	//如果不析构则设为nullptr
			delete q;
			length--;
        }
	}

	/**
	* @brief 从第start个后查找data为value的节点的序号
	*
	* @param start 从第几个始找
	* @param value 搜索的数据
	*
	* @return 返回node的序号;没找到则返回-1
	*/
	int indexOf(T value, int start = 0) {
		Node<T>* p = head->next;
		for (int i = 0; i < length; i++) {
			if (i >= start && p->data == value)
				return i;
			p = p->next;
		}
		return -1;
	}

	/**
	* @brief 找node的序号 (序号从0开始)
	*
	* @param node 要找的节点
	*
	* @return 返回node的序号;没找到则返回-1
	*/
	int indexOf(Node<T>* node) {
		Node<T>* p = head->next;
		for (int i = 0; i < length; i++) {
			if (p == node) return i;
			p = p->next;
		}
		return -1;
	}

    /**
    * @brief 找data的序号 (序号从0开始)
    *
    * @param d 要找的data
    *
    * @return 返回node的序号;没找到则返回-1
    */
    int indexOf(T* d) {
        Node<T>* p = head->next;
        for (int i = 0; i < length; i++) {
            if (p->data == d) return i;
            p = p->next;
        }
        return -1;
    }

	/**
	* @brief 链表转数组
	*
    * @return 返回长度为length的数组指针 如果长度为0则返回空指针
	*/
	T* toArray() {
        if(length == 0) return nullptr;
		T* result = new T[length];
		Node<T>* p = head->next;
		for (int i = 0; i < length; i++) {
			result[i] = p->data;
			p = p->next;
		}
		return result;
	}

	/**
	* @brief 遍历每个节点并对其使用fun
	*
	* @param fun(Node<T>*,int) 对每个节点的操作,int对应节点的index
	*/
	void forEach(void(*fun)(Node<T>*, int)) {
		Node<T>* p = head->next;
		for (int i = 0; i < length; i++) {
			Node<T>* q = p;
			p = p->next;
			fun(q, i);
		}
	}
};
#endif
