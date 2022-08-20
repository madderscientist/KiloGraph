#ifndef POINT_H
#define POINT_H

#include <QPushButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QDebug>
#include <QtMath>
#include <queue>
#include "KG.h"
#include "tiplabel.h"
#define RADIUS 30

class Point : public QPushButton
{
    Q_OBJECT
private:
    double getZoomTime();

public:
    explicit Point(QWidget *parent = nullptr);
    ~Point();
    V* v = nullptr;             // 绑定的V
    QPointF location;           // Scene中的坐标
    QPoint clickpos;            // 被按的位置

    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);

    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

    void moveOnScene(int,int);  // 设置在Scene里面的位置（绝对位置
    void moveOnScene(QPointF);
    void moveF(QPointF);
    void setViewSize(int = 0);  // 传值则强制设定大小, 不传则按倍率刷新为默认大小

    char selected = 0;
    char father = 0;
    char hidden = 0;
    char hidechild = 0;
    void refreshStyle(char Selected = -1, char Father = -1, char Hidden = -1, char Hidechild = -1);

    QPointF speed;
signals:

};

#endif // POINT_H
