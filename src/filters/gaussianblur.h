#ifndef FILTER_GAUSSIAN_BLUR_H
#define FILTER_GAUSSIAN_BLUR_H

#include "filters/filter.h"

/**
 * A filter for grayscaling an image.
 */
class GaussianBlur : public Filter2D {
    public:
	GaussianBlur(double radius);
	void apply(Image* image);
	
    private:
	double radius;
};

#endif

