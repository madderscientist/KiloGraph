#ifndef TDETAIL_H
#define TDETAIL_H

#include <QWidget>
#include "point.h"

namespace Ui {
class TDetail;
}
// 老师查看详情
class TDetail : public QWidget {
    Q_OBJECT
public:
    explicit TDetail(Point *,QWidget *parent = nullptr);
    ~TDetail();
private:
    Point* p;
    Ui::TDetail *ui;
};

#endif // TDETAIL_H
