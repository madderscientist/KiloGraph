#ifndef CARD_H
#define CARD_H

#include <QLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QMouseEvent>
#include <QDebug>
#define CardRadius 20
#define CardW 400

// 顶部标题栏 含关闭按钮
class TopBar : public QWidget {
    Q_OBJECT
public:
    explicit TopBar(QString title = "", QWidget* parent = nullptr);
    QLabel* Title;
    QPushButton* off;                       // 关闭按钮
    void setTitie(QString title);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
private:
    QPoint clickpos;
};

// 底部改变大小栏
class BottomResize : public QWidget {
    Q_OBJECT
public:
    explicit BottomResize(QWidget* parent = nullptr);
    void mouseMoveEvent(QMouseEvent*);
};

// 卡片容器 添加部件请往Layout中addWidget
class Card : public QWidget {
    Q_OBJECT
public:
    explicit Card(QString title = "", QWidget *parent = nullptr);
    TopBar* top;
    BottomResize* bottom;
    QVBoxLayout *Layout;
    void resizeEvent(QResizeEvent*);
};

#endif // CARD_H
