#ifndef SDETAIL_H
#define SDETAIL_H

#include "card.h"
#include "page.h"

class SDetail : public Card
{
    Q_OBJECT
public:
    explicit SDetail(Point* p, Page *parent = nullptr);
    Point* P;

signals:

};

#endif // SDETAIL_H
