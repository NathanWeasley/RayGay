#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "lightsource.h"
#include <vector>

class Circle;
class object;

/// Lightsource that produces soft shadows
class Arealight : public Lightsource {

    public:
	/// Constructor
	Arealight(const Vector& pos, const Vector& dir, double radius, int num, double jitter);
	virtual ~Arealight();
	Lightinfo getLightinfo(const Intersection& inter, const Vector& normal, const SpaceSubdivider& space) const;
	const Vector& getPosition() const;
        void transform(const Matrix& m);

    private:
	std::vector<Circle*> circles;
	mutable std::vector<object*> hints;
	std::vector<double> ts;
	double jitter;
	int num;
	Vector position;
	Vector getPosition(int i) const;
};

#endif
