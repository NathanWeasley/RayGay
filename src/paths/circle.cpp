
#include <cassert>
#include <iostream>
#include "math.h"

#include "circle.h"
#include "boundingbox.h"

/**
 * Constructs a circle path
 *
 * @param center Center of the circle
 * @param radius Radius of the circle
 * @param normal Normal of the plane the circle should live in
 */
Circle::Circle(const Vector& center, double radius, const Vector& normal) {
    c = center;
    r = radius;
    n = normal;
    n.normalize();
    Vector y = Vector(0,1,0);
    Vector x = Vector(1,0,0);
    Vector a = n == y ? x : y;
    m = Matrix::matrixOrient(n,Vector::xProduct(a,n));
    orient = m;
    m = m * Matrix::matrixTranslate(center);
}

Vector Circle::getPoint(double t) const {
    double rad = M_2PI * t;
    Vector result = Vector(cos(rad)*r,sin(rad)*r,0);
    return m * result;
}

Vector Circle::getTangent(double t) const {
    double rad = M_2PI * t;
    Vector result = Vector(-sin(rad),cos(rad),0);
    return orient * result;
}

void Circle::transform(const Matrix& m) {
    c = m * c;
    n = m.extractRotation() * n;
}

void Circle::test() {
    int num = 100;
    Vector points[100];
    BoundingBox b;

    /* Test at origin */
    Circle c = Circle(Vector(0,0,0),10,Vector(0,0,1));
    assert(c.isClosed());
    c.getPoints(num,points);
    b = BoundingBox(Vector(-11,-11,-1),Vector(11,11,1));
    assert(b.inside(points,num));
    

    /* Test translated */
    c = Circle(Vector(10,10,0),10,Vector(0,0,1));
    assert(c.isClosed());
    c.getPoints(num,points);
    b = BoundingBox(Vector(-1,-1,-1),Vector(21,21,1));
    assert(b.inside(points,num));

    /* Test direction along x */
    c = Circle(Vector(0,0,0),10,Vector(1,0,0));
    assert(c.isClosed());
    c.getPoints(num,points);
    b = BoundingBox(Vector(-1,-11,-11),Vector(1,11,11));
    assert(b.inside(points,num));

    /* Test direction along y */
    c = Circle(Vector(0,0,0),10,Vector(0,1,0));
    assert(c.isClosed());
    c.getPoints(num,points);
    b = BoundingBox(Vector(-11,-1,-11),Vector(11,1,11));
    assert(b.inside(points,num));

    /* Test tangent */
    c = Circle(Vector(0,0,0),10,Vector(0,1,0));
    assert(c.getPoint(0) == Vector(-10,0,0));
    assert(c.getTangent(0) == Vector(0,0,1));
    assert(c.getPoint(0.25) == Vector(0,0,10));
    assert(c.getTangent(0.25) == Vector(1,0,0));
    assert(c.getPoint(0.5) == Vector(10,0,0));
    assert(c.getTangent(0.5) == Vector(0,0,-1));
    assert(c.getPoint(0.75) == Vector(0,0,-10));
    assert(c.getTangent(0.75) == Vector(-1,0,0));

    c = Circle(Vector(0,10,0),10,Vector(0,1,0));
    assert(c.getPoint(0) == Vector(-10,10,0));
    assert(c.getTangent(0) == Vector(0,0,1));
    assert(c.getPoint(0.25) == Vector(0,10,10));
    assert(c.getTangent(0.25) == Vector(1,0,0));
    assert(c.getPoint(0.5) == Vector(10,10,0));
    assert(c.getTangent(0.5) == Vector(0,0,-1));
    assert(c.getPoint(0.75) == Vector(0,10,-10));
    assert(c.getTangent(0.75) == Vector(-1,0,0));


    /* Done */
    std::cout << "Circle::test() done." << std::endl;
}
