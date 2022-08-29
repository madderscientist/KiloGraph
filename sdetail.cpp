#include "sdetail.h"

SDetail::SDetail(Point* p, Page *parent)
    : Card(QString::number(p->v->id)+"号节点详情", parent), P(p)
{

}
