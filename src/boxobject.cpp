
#include "boxobject.h"
#include "boundingbox.h"
#include "material.h"
#include "vector.h"
#include "rgb.h"
#include "intersection.h"
#include "ray.h"
#include "math.h"
#include "constants.h"

BoundingBoxobject::BoundingBoxobject(BoundingBox b, Material m) {
    _box = b;
    _material = m;
}

BoundingBoxobject::BoundingBoxobject(Vector pos, double width, double height, double depth, Material m) {
    _box = BoundingBox(pos, Vector(pos[0]+width,pos[1]+height,pos[3]+depth));
    _material = m;
}
	

RGB BoundingBoxobject::getDiffuseColor(const Vector& p) {
    Vector pt = _invTransform * p;
    pt = p;
    static double tilesize = 100.0;
    double size = 1.0 / tilesize;
    //double size = 0.02;
    int res = int(floor(size*pt[0] + EPSILON) +
	      floor(size*pt[1] + EPSILON) +
	      floor(size*pt[2] + EPSILON));

    return (res % 2) ? RGB(1.0,1.0,1.0) : RGB(0.2,0.2,1.0);
    return _material.getDiffuseColor();
}

Material BoundingBoxobject::getMaterial() {
    return _material;
}

bool BoundingBoxobject::onEdge(const Vector &p) {
    return _box.onEdge(_invTransform * p);
}

bool BoundingBoxobject::inside(const Vector &p) {
    return _box.inside(_invTransform * p);
}

Intersection BoundingBoxobject::_intersect(const Ray& ray) {
    Vector o = _invTransform * ray.origin;
    Vector d = (_invTransform * (ray.direction + ray.origin) ) - o;
    d.normalize();
    Ray r = Ray(o, d, ray.indice_of_refraction);
    Intersection i = _box.intersect(r);
    i.local_point[0] = i.point[0];
    i.local_point[1] = i.point[1];
    i.local_point[2] = i.point[2];
    i.point = _transform * i.point;
    return i;
}

void BoundingBoxobject::transform(const Matrix& m) {
    _transform = m;
    _invTransform = m.inverse();
}

Vector BoundingBoxobject::normal(const Intersection& i) {
    // TODO: normalen skal roteres tilbage til scenens koordinatsystem, men ikke translateres
    //return _transform * _box.normal(i.local_point);
    return _transform * _box.normal(_invTransform * i.point);
}

bool BoundingBoxobject::intersects(BoundingBox& box) {
    // TODO: Implement
}
