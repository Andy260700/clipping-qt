#ifndef QPOINTHASHER_H
#define QPOINTHASHER_H
#include <iostream>
#include <QPoint>

struct QPointHasher
{
    size_t operator()(QPoint const&) const;
};

#endif // QPOINTHASHER_H
