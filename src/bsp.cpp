
#include <cassert>
#include <iostream>

#include "bsp.h"
#include "object.h"
#include "ray.h"
#include "intersection.h"
#include "boundingbox.h"
#include "constants.h"

// For test
#include "sphere.h"

BSP::BSP() {
    cutplane_dimension = 0;
    cutplane_value = 0;
    lower == NULL;
    higher == NULL;
}

void BSP::addObject(object* obj) {
    objects.push_back(obj);
}

void BSP::prepare() {
    if (objects.size() > BSP_MAX) {
	// Find the cutplane_dimension and cutplane_value
	BoundingBox bbox = enclosure();
	//std::cout << bbox << std::endl;
	cutplane_dimension = largestDimension(bbox);
	cutplane_value = ((bbox.maximum()[cutplane_dimension]) + 
                          (bbox.minimum()[cutplane_dimension])) / 2.0;

	lower = new BSP();
	higher = new BSP();

	int size = objects.size();

	// Put all objects into lower- or higher_objects
	for(int i = 0; i < size; i++) {
	    object* obj = objects[i];
	    BoundingBox bbox = obj->boundingBoundingBox();
	    int cut_val = bbox.cutByPlane(cutplane_dimension, cutplane_value);
	    if (cut_val == -1) {
		lower->addObject(obj);
	    } else if (cut_val == 1) {
		higher->addObject(obj);
	    } else {
		lower->addObject(obj);
		higher->addObject(obj);
	    }
	}
	
	if (lower->objects.size() == size || higher->objects.size() == size) {
	    // Objects couldn't be subdivided
	    delete lower;
	    delete higher;
	} else {
	    objects.clear();
	    assert(objects.size() == 0);
	    // Recursive prepare()
	    lower->prepare();
	    higher->prepare();
	}
    }
}

Intersection BSP::intersect(const Ray& ray) const {
    return intersect(ray,0,HUGE_DOUBLE);
}

Intersection BSP::intersectForShadow(const Ray& ray) const {
    // TODO: Optimize
    return intersect(ray,0,HUGE_DOUBLE);

}

Intersection BSP::intersectForShadow(const Ray& ray, const object* hint) const {
    Intersection result = hint->intersect(ray);
    return result.intersected ? result : intersectForShadow(ray);
}



/*******************
 * Private stuff   *
 *******************/
Intersection BSP::intersect(const Ray& ray, double min_t, double max_t) const {
    Intersection result = Intersection();
    if (objects.size() > 0) {
	double cur_t = HUGE_DOUBLE;
	for(int i=0; i < objects.size(); i++) {
	    object* obj = objects[i];
	    Intersection i = obj->intersect(ray);
	    if (i.intersected && i.t < cur_t) {
		result = i;
		cur_t = i.t;
	    }
	}
    } else {
        result = intersect_recurse(ray,min_t,max_t);
    }
    return result;
}

Intersection BSP::intersect_recurse(const Ray& ray, double min_t, double max_t) const {
    if (max_t <= min_t) return Intersection();

    //TODO: Use max_t somewhere
    Vector o = ray.origin + min_t * ray.direction;

    if (o[cutplane_dimension] < cutplane_value && 
	ray.direction[cutplane_dimension] <= 0) {
        return lower->intersect(ray,min_t,max_t);
    } else if (o[cutplane_dimension] > cutplane_value && 
	ray.direction[cutplane_dimension] >= 0) {
        return higher->intersect(ray,min_t,max_t);
    } else {
	// Ray intersects cutplane (TODO: Check for zero-division case)
	double intersect_t = (cutplane_value - ray.origin[cutplane_dimension]) / ray.direction[cutplane_dimension];
	if (intersect_t > max_t) intersect_t = max_t;
	if (intersect_t < min_t) intersect_t = min_t;
	Intersection low_intersection = lower->intersect(ray,min_t,intersect_t);
	Intersection high_intersection = higher->intersect(ray,intersect_t,max_t);
	if (low_intersection.t < high_intersection.t) {
	    return low_intersection;
	} else {
	    return high_intersection;
	}
    }
}



BoundingBox BSP::enclosure() const {
    assert(objects.size() > 0);
    BoundingBox result = objects[0]->boundingBoundingBox(); 
    for(int i = 1; i < objects.size(); i++) {
        result = BoundingBox::doUnion(result,objects[i]->boundingBoundingBox());
    }
    return result;
}

int BSP::largestDimension(const BoundingBox& box) {
    double x = box.maximum()[0] - box.minimum()[0];
    double y = box.maximum()[1] - box.minimum()[1];
    double z = box.maximum()[2] - box.minimum()[2];
    double max = MAX(x,MAX(y,z));
    if (IS_EQUAL(x,max)) {
	    return 0;
    } else if (IS_EQUAL(y,max)) {
	    return 1;
    } else if (IS_EQUAL(z,max)) {
	    return 2;
    }
    // Throw an exception
}

void BSP::test() {
    BSP bsp;
    for(int x = -10; x <= 10; x++) {
       for(int y = -10; y <= 10; y++) {
           for(int z = -10; z <= 10; z++) {
	      Sphere* sx = new Sphere(Vector(x*20,y*20+50,z*20),10,Material(RGB(0.8,0.8,0.8),0.7,RGB(1.0,1.0,1.0),0.80,40));
	      bsp.addObject(sx);
	   }
 	}
    }
    BoundingBox box = bsp.enclosure();
    assert(IS_EQUAL(box.minimum()[0],-210));
    assert(IS_EQUAL(box.minimum()[1],-160));
    assert(IS_EQUAL(box.minimum()[2],-210));
    assert(IS_EQUAL(box.maximum()[0],210));
    assert(IS_EQUAL(box.maximum()[1],260));
    assert(IS_EQUAL(box.maximum()[2],210));

    bsp.addObject(new Sphere(Vector(0,-500,0),10,Material(RGB(0.8,0.8,0.8),0.7,RGB(1.0,1.0,1.0),0.80,40)));
    assert(largestDimension(bsp.enclosure()) == 1);

    std::cout << "BSP::test() done." << std::endl;
}
