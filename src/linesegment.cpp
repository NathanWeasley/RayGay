
#include "linesegment.h"
#include "vector.h"
#include <cassert>

Linesegment::Linesegment(const Vector& begin, const Vector& end) {
    b = begin;
    e = end;
    tgt = end - begin;
    tgt.normalize();
}

Vector Linesegment::getPoint(double t) const {
    return (1-t)*b + t*e;
}

inline
Vector Linesegment::getTangent(double t) const {
    return tgt;
}

void Linesegment::test() {
    Vector b1 = Vector(-10,-10,-10);
    Vector b2 = Vector(10,10,10);
    Linesegment l = Linesegment(b1,b2);
    assert(b1 == l.getPoint(0));
    assert(b2 == l.getPoint(1));

    Vector p[3];
    l.getPoints(3,p);
    assert(p[0] == b1);
    assert(p[1] == Vector(0,0,0));
    assert(p[2] == b2);

    l.getTangents(3,p);
    assert(p[0] == p[1]);
    assert(p[1] == p[2]);
    Vector n = Vector(1,1,1);
    n.normalize();
    assert(p[0] == n);

    assert(!l.isClosed());
}
