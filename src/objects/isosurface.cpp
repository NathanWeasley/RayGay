
#include <iostream>
#include <cassert>
#include "objects/isosurface.h"
#include "intersection.h"
#include "boundingbox.h"
#include "math/vector2.h"

using namespace std;

IsoSurface::IsoSurface(uint steps, double accuracy, double iso, Material* mat) : Object(mat) {
    this->steps = steps;
    this->accuracy = accuracy;
    this->iso = iso;
}

void IsoSurface::_fullIntersect(const Ray& world_ray, const double t, Intersection& result) const {
    Ray ray = rayToObject(world_ray);
    Vector p = ray.getPoint(t*ray.t_scale);
    result = Intersection(p,t*ray.t_scale,normal(p),Vector2(0,0));
    intersectionToWorld(result);
}

#define func(x) (evaluateFunction(local_ray.getPoint(x)) - iso)

double IsoSurface::_fastIntersect(const Ray& world_ray) const {

    Ray local_ray = rayToObject(world_ray);
    double res = -1;
    
    const BoundingBox& bbox = this->_boundingBoundingBox();
    Vector2 inout = bbox.intersect(local_ray);
    double t_begin = fmax(inout[0],accuracy);
    double t_end = inout[1] + accuracy;
    if (t_end > t_begin) {
	
	int start_sign = SIGN(func(t_begin));
	double t_step = (t_end - t_begin) / double(steps);

	for(double t = t_begin; t <= t_end; t += t_step) {
	    if (SIGN(func(t)) != start_sign) {
		res = refine(local_ray,t-t_step,t);
		break;
	    }
	}
    }

    if (res > EPSILON) {
	return res / local_ray.t_scale;
    } else {
	return -1;
    }
}

#undef func

#define func(x) (iso - evaluateFunction(ray.getPoint(x)))
#define MAX_ITER 100
double IsoSurface::refine(const Ray& ray, double t_begin, double t_end) const {
    double f_t_begin = func(t_begin);
    double f_t_end = func(t_end);
    double f_t_mid, t_mid;
    uint i = 0;

    //assert(!SAME_SIGN(f_t_begin, f_t_end));

    while (i++ < MAX_ITER) {
	t_mid = ( f_t_end * t_begin - f_t_begin * t_end ) / ( f_t_end - f_t_begin );
	f_t_mid = func(t_mid);

	if (fabs(f_t_mid) < accuracy / 100) {
	    return t_mid;
	}

	if (SAME_SIGN(f_t_begin, f_t_mid)) {
	    t_begin = t_mid;
	    f_t_begin = f_t_mid;
	} else {
	    t_end = t_mid;
	    f_t_end = f_t_mid;
	}

    }
    return 0;
}
#undef func


/**
 * Finds the surface normal at a point.
 * 
 * @param p a surface point in object space
 * @return the surface normal at point
 */
Vector IsoSurface::normal(const Vector& p) const {
    double off = accuracy;
    double x = evaluateFunction(p - Vector(off,0,0)) - 
	       evaluateFunction(p + Vector(off,0,0));
    double y = evaluateFunction(p - Vector(0,off,0)) - 
	       evaluateFunction(p + Vector(0,off,0));
    double z = evaluateFunction(p - Vector(0,0,off)) - 
	       evaluateFunction(p + Vector(0,0,off));
    Vector normal = Vector(0.5*x,0.5*y,0.5*z);
    normal.normalize();
    return  normal;
}

bool IsoSurface::intersects(const BoundingBox& b) const {
    return b.inside(boundingBoundingBox());
}

void IsoSurface::transform(const Matrix& m) {
    Transformer::transform(m);
}

BoundingBox IsoSurface::boundingBoundingBox() const {
    BoundingBox result = bboxToWorld(_boundingBoundingBox());
    result.grow(20*EPSILON);
    return result;
}

