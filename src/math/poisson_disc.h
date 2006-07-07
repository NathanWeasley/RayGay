
#ifndef MATH_POISSON_DISC
#define MATH_POISSON_DISC

#include "math/vector2.h"

class PoissonDiscDistribution
{
    public:
	/**
	 * Creates a set of random points within [0,w] &cross; [0,h] that
	 * all are at least a distance of 2r from each other.
	 * 
	 * @param result must have room for num Vector2's
	 * @return the number of points found whis is &leq; num
	 */
	static int createSet(double w, double h, double r, int num, Vector2* result);
};

#endif