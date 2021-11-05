#include "qpointhasher.h"

size_t QPointHasher::operator()(QPoint const& p) const
{
    return qHash(p);
}
