
#include "cameras/latlong.h"
#include "ray.h"
#include "math/functions.h"

LatLong::LatLong() : Camera () {
}

Ray LatLong::_getRay(const double x, const double y) 
{
    double theta = M_PI * y;
    double phi = M_2PI * x - M_PI;
    Vector dir = Vector(sin(theta)*cos(phi), -cos(theta), sin(theta)*sin(phi));
    dir = dir;
    dir.normalize();
    return Ray(position, dir, 1.0);
}

