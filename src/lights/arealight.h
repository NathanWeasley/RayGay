#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "lights/lightsource.h"
#include "lights/shadowcache.h"
#include <vector>

class Circle;
class Object;

/// Lightsource that produces soft shadows
class Arealight : public Lightsource {

    public:
	/// Constructor
	Arealight(const Vector& pos, const Vector& dir, double radius, int num, double jitter);
	virtual ~Arealight();
	void getLightinfo(const Intersection& inter, KdTree* space, Lightinfo* info, uint32_t depth) const;
        void transform(const Matrix& m);

    private:
	std::vector<Circle*> circles;
	// TODO: Not threadsafe! Make this thread_local. Identical problem in skylight.
	mutable std::vector<ShadowCache> shadowcaches;
	std::vector<double> ts;
	double jitter;
	int num;

	Vector getPosition(int i) const;
	bool probeSublight(int i, const Intersection& inter, KdTree* space, uint32_t depth) const;
};

#endif
