#ifndef STUDENT_H
#define STUDENT_H
#include <map>
#include "KG.h"
#include "sdetail.h"
using namespace std;

class Page;
// 需要存文件的 原始数据
class StuProgress {
public:
    StuProgress();
    ~StuProgress();
    // long为题目id. string记录做题历史: 第i个字符代表第(i+1)次做题正确(1)与否(0)
    map<long, string>* stu;
    string& id(long);       // 快捷访问
    static bool SPread(StuProgress*, string path);
    static bool SPwrite(StuProgress*, string path);
};

// 一个节点的掌握情况
struct Mastery {
    Task** tasks = nullptr;       // 其下所有题目
    int taskNum=0;        // 其下的题目总数
    int rightNum=0;       // 最后一次做对的题目数
    int doneNum=0;        // 做过的题目数
    ~Mastery(){delete[]tasks;}
};

// 管理学生做题的所有信息
/*
 * 文件导入时：dir赋值->读取StuProgress->初始化Mastery
 * 新建进度文件：dir置空->StuProgress空初始化->初始胡Mastery
 * 开始做题：建立在sp和mastery已经new的基础上，new DoTask
*/
class Student {
public:
    Student(Page*, QString path);
    ~Student();
    Page* page;                     // 便于访问kg
    QString dir = "";               // 文件路径 区分新建和打开

    Mastery* mastery = nullptr;     // 知识点情况数组 第i个代表第i个节点的情况
    StuProgress* sp;                // 文件 原始数据 有文件才new
    DoTask* dotask = nullptr;       // 做题界面 做题才new 关闭窗口为析构

    void save(bool saveAs = false); // 根据dir存文件
    void exercise(Point*);          // 开始做题 做题细节在DoTask中实现
    void refreshMastery();          // 更新所有mastery

private:
    void iniMastery();              // 初始化mastery 因为学生界面为只读 因此数组长度可定
};

#endif // STUDENT_H
