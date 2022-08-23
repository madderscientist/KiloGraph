#ifndef TDETAIL_H
#define TDETAIL_H

#include "card.h"
#include "point.h"
#include "page.h"

class myLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit myLineEdit(QWidget* =nullptr);
    void focusOutEvent(QFocusEvent*);
signals:
    void focusOut();
};

class myPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit myPlainTextEdit(QWidget* =nullptr);
    void focusOutEvent(QFocusEvent*);
signals:
    void focusOut();
};


class TDetail : public Card
{
    Q_OBJECT
public:
    explicit TDetail(Point* p, Page *parent = nullptr);
    ~TDetail();
    Point* P;
    myLineEdit* title;
    myPlainTextEdit* description;
    QListWidget* Es;
    void mousePressEvent(QMouseEvent*);
    void iniList();
    void clearList();
signals:
    void startAddE();
    void stopAddE();
};

// 老师边列表的一项
class TEdgeInf : public QListWidgetItem {
public:
    explicit TEdgeInf(V*,QListWidget* parent);
    ~TEdgeInf();
    V* v;
    QWidget* w;
    QHBoxLayout* Layout;
    QLabel* ano;
    QPlainTextEdit* detail;
    QPushButton* save;
    QPushButton* cancel;
    void fold();
    void expand();
};

#endif // TDETAIL_H
