
#include <cassert>

#include "lights/arealight.h"
#include "paths/circle.h"
#include "intersection.h"
#include "math/vector.h"

class KdTree;

/**
 * Constructs an area light
 *
 * @param pos Position
 * @param dir Direction
 * @param radius The radius
 * @param num Number of samples
 * @param jitter How many percent the light can jitter with value in [0,1]
 */
Arealight::Arealight(const Vector& pos, const Vector& dir, double radius, int num, double jitter) : Lightsource(pos) {
    assert(num > 1);

    this->num = num;
    this->jitter = jitter;

    /* De N punkter skal placeres p� N koncentriske b�nd omkring P. Disse
     * N b�nd skal have samme areal A, s� punkterne er d�kker cirklens 
     * areal med bedste fordeling. Dette A er PI * R^2 / N.
     *
     * r(n) for 0 <= n <= N bliver dermed n * A / PI dvs.  
     * r(n) = sqrt(n/N)*R for n mellem 0 og N.
     */
    for(int i = 0; i < num; i++) {
	double r = radius*sqrt(double(i)/(num-1));
	circles.push_back(new Circle(pos,r,dir));
	ts.push_back(RANDOM(0,1));
	shadowcaches.push_back(ShadowCache());
    }
}

Arealight::~Arealight() {
}

void Arealight::transform(const Matrix& m) {
    Lightsource::transform(m);
    for(int i = 0; i < num; i++) {
	circles[i]->transform(m);
    }
}

Vector Arealight::getPosition(int i) const {
    assert(i < num);
    double j = jitter * RANDOM(0,1);
    double t = (ts[i] + j) - int(ts[i] + j);
    return circles[i]->getPoint(t);
}

void Arealight::getLightinfo(const Intersection& inter, KdTree* space, Lightinfo* info, unsigned int depth) const {
    Vector direction_to_light;
    info->direction_to_light = position - inter.getPoint();
    info->direction_to_light.normalize();
    info->cos = info->direction_to_light * inter.getNormal();

    if (info->cos > 0.0) {
	int count = 0;
	for(int i = 0; i < num; i++) {
	    direction_to_light = getPosition(i) - inter.getPoint();
	    double dist_to_light = direction_to_light.length();
	    if (IS_ZERO(dist_to_light)) {
		continue;
	    }
	    direction_to_light *= 1.0/dist_to_light;

	    Ray ray_to_light = Ray(inter.getPoint(),direction_to_light,-1.0);

	    bool occluded = shadowcaches[i].occluded(ray_to_light,dist_to_light,depth,space);

	    if (!occluded) {
		count++;
	    }
	}
	info->intensity = double(count) / num;
    }
}
