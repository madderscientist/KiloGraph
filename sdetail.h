#ifndef SDETAIL_H
#define SDETAIL_H

#include "card.h"
#include "KG.h"

class Page;
class Point;
// 学生界面 内容不可编辑 查看统计信息 开始做题
// 和TDetail用多态存Point里面（两者不可能共存
class SDetail : public Card
{
    Q_OBJECT
public:
    explicit SDetail(Point* p, Page *parent = nullptr);
    Point* P;
    QLabel* statistics;
    void Statistic();       // 统计数据
};

/*===========以下做题===========*/

// 做题窗口 推题算法
/* 学生界面点击开始做题则创建
 * 创建时根据Student里面的总题数创建vtask数组
*/
class DoTask : public Card
{
    Q_OBJECT
public:
    // 构造函数通过parent->Student来访问数据
    explicit DoTask(Point* p, Page *parent = nullptr);
    ~DoTask();
    QLineEdit* ans;
    QPlainTextEdit* taskText;   // 题干（只读
    QLabel* judge;              // 判断对错

    V* v;
    float* weight;           // v的所有task的权的一维数组
    Task* currentTask = nullptr;
    void pickTask();        // 根据v挑选题目 题目存到currentTask
    void showTask();        // 根据currentTask展示题目
private:
    bool iniData();         // 数据初始化（和界面初始化分开
    void updateWeight(bool);// 更新权值

};

#endif // SDETAIL_H
