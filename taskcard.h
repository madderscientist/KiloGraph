#ifndef TASKCARD_H
#define TASKCARD_H

#include "card.h"
#include "KG.h"
#include <QScrollArea>


class Page;
class TaskCard : public Card {
    Q_OBJECT
public:
    explicit TaskCard(Page* = nullptr);
    ~TaskCard();
    QListWidget* Ts;
    void iniList();
    void clearList();
    void keyPressEvent(QKeyEvent*);
};

class TaskInf : public QListWidgetItem {
public:
    explicit TaskInf(Task*,TaskCard* parent, QListWidget* list);
    ~TaskInf();
    void offself();
    Page* page;
    Task* t;
    QWidget* w;
    myPlainTextEdit* taskText;
    myLineEdit* ans;
    QHBoxLayout* BindVs;

    QHBoxLayout* layout1;
    QHBoxLayout* layout2;
    void ifFold(bool);
    void startBindV();
    void stopBindV();
private:
    void readTask();
    void saveTask();
    void clearVs();
    void iniVs();
};

// 一个显示V标题和删除按钮的胶囊
class BindV : public QWidget {
    Q_OBJECT
public:
    explicit BindV(V*, TaskInf* = nullptr);
    V* v;
    Page* page;
    void mousePressEvent(QMouseEvent*);
};
#endif // TASKCARD_H
