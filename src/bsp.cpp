
#include <cassert>
#include <iostream>
#include <list>

#include "bsp.h"
#include "object.h"
#include "ray.h"
#include "intersection.h"
#include "boundingbox.h"

// For test
#include "sphere.h"

BSP::BSP() {
    cutplane_dimension = 0;
    cutplane_value = 0;
}

void BSP::addObject(object* obj) {
    objects.push_back(obj);
}

void BSP::prepare() {
    if (objects.size() > BSP_MAX) {
	// Find the cutplane_dimension and cutplane_value
	BoundingBox bbox = enclosure();
	//std::cout << bbox << std::endl;
	//cutplane_dimension = largestDimension(bbox);
	
	/*cutplane_value = ((bbox.maximum()[cutplane_dimension]) + 
                          (bbox.minimum()[cutplane_dimension])) / 2.0;*/

	//cutplane_value = median(cutplane_dimension);

	Vector best_measure = Vector(0,HUGE_DOUBLE,0);
	int best_dim = -1;
	double best_val = 1;
	for(int i = 0; i < 3; i++) {
	    double val = median(i);
	    Vector measure = measureSplit(i,val);
	    if (measure[1] < best_measure[1] &&
		measure[0] <  objects.size() &&
		measure[2] <  objects.size()) {
		best_dim = i;
		best_val = val;
		best_measure = measure;
	    }
	}
	if (best_dim != -1) {
	    cutplane_dimension = best_dim;
	    cutplane_value = best_val;
	} else {
	    cutplane_dimension = largestDimension(bbox);
	    cutplane_value = median(cutplane_dimension);
	}

	lower = new BSP();
	higher = new BSP();

	unsigned int size = objects.size();

	// Put all objects into lower- or higher_objects
	int l = 0; int m = 0; int h = 0;
	for(unsigned int i = 0; i < size; i++) {
	    object* obj = objects[i];
	    BoundingBox bbox = obj->boundingBoundingBox();
	    int cut_val = bbox.cutByPlane(cutplane_dimension, cutplane_value);
	    if (cut_val == -1) {
		lower->addObject(obj);
		l++;
	    } else if (cut_val == 1) {
		higher->addObject(obj);
		h++;
	    } else {
		lower->addObject(obj);
		higher->addObject(obj);
		m++;
	    }
	}
	
//	if (true) {
	if (lower->objects.size() == size || higher->objects.size() == size) {
	    // Objects couldn't be subdivided
	    delete lower;
	    delete higher;
	} else {
	//    std::cout << size << ": " << l << " " << m << " " << h << std::endl;
	    objects.clear();
	    assert(objects.size() == 0);
	    // Recursive prepare()
	    lower->prepare();
	    higher->prepare();
	}
    }
}

inline
Intersection BSP::intersect(const Ray& ray) const {
    return intersect(ray,0,HUGE_DOUBLE);
}

inline
Intersection BSP::intersectForShadow(const Ray& ray) const {
    return intersectForShadow(ray,0,HUGE_DOUBLE);
}

Intersection BSP::intersectForShadow(const Ray& ray, const object* hint) const {
    Intersection result = hint->intersect(ray);
    if (result.isIntersected()) {
	return result;
    } else {
        return intersectForShadow(ray);
    }
}

double BSP::median(int d) const {
    std::list<double> L;
    for(unsigned int i = 0; i < objects.size(); i++) {
	    object* obj = objects[i];
	    BoundingBox bbox = obj->boundingBoundingBox();
	    double c = (bbox.maximum()[d] + bbox.minimum()[d]) / 2.0;
	    L.push_back(c);
    }
    L.sort();
    unsigned int size = L.size();
    assert(size == objects.size());
    // Return L[size/2]
    unsigned int i = 0;
    for (std::list<double>::iterator h = L.begin(); h != L.end(); h++) {
	if (i++ > size/2) return *h;
    }
    exit(0);
}

Vector BSP::measureSplit(int dim, double val) const {
    Vector result = Vector(0,0,0);
    for(unsigned int i = 0; i < objects.size(); i++) {
	object* obj = objects[i];
	BoundingBox bbox = obj->boundingBoundingBox();
	int cut_val = bbox.cutByPlane(dim, val);
	if (cut_val == -1) {
	    result[0]++;
	} else if (cut_val == 1) {
	    result[2]++;
	} else {
	    result[1]++;
	}
    }
    return result;
}

/*******************
 * Private stuff   *
 *******************/
const Intersection BSP::intersectForShadow(const Ray& ray, const double min_t, const double max_t) const {
    if (objects.size() > 0) {
        Intersection tmp;
	for (unsigned int i=0; i < objects.size(); i++) {
	    tmp = objects[i]->intersect(ray);
	    if (tmp.isIntersected()) {
		return tmp;
	    }
	}
	return Intersection();
    } else {
        return intersectForShadow_recurse(ray,min_t,max_t);
    }
}

const Intersection BSP::intersect(const Ray& ray, const double min_t, const double max_t) const {
    Intersection result = Intersection();
    if (objects.size() > 0) {
        Intersection tmp;
	double cur_t = HUGE_DOUBLE;
	for (unsigned int i=0; i < objects.size(); i++) {
	    tmp = objects[i]->intersect(ray);
	    if (tmp.isIntersected() && tmp.getT() < cur_t) {
		result = tmp;
		cur_t = tmp.getT();
	    }
	}
    } else {
        result = intersect_recurse(ray,min_t,max_t);
    }
    return result;
}

const Intersection BSP::intersect_recurse(const Ray& ray, const double min_t, const double max_t) const {
    if (max_t <= min_t) return Intersection();

    //Vector o = ray.getOrigin() + min_t * ray.getDirection();
    double rd_dim = ray.getDirection()[cutplane_dimension];
    double o_dim = ray.getOrigin()[cutplane_dimension] + min_t * rd_dim;
    
    if (o_dim < cutplane_value && rd_dim <= 0) {
        return lower->intersect(ray,min_t,max_t);
    } else if (o_dim > cutplane_value && rd_dim >= 0) {
        return higher->intersect(ray,min_t,max_t);
    } else {
	double intersect_t = (cutplane_value - ray.getOrigin()[cutplane_dimension]) * ray.getInverseDirection()[cutplane_dimension];
	if (intersect_t > max_t) { intersect_t = max_t; }
	if (intersect_t < min_t) { intersect_t = min_t; }

	//Intersection intersection1 = lower->intersect(ray,min_t,intersect_t);
	//Intersection intersection2 = higher->intersect(ray,intersect_t,max_t);
	
	Intersection intersection1;
	Intersection intersection2;

	if (o_dim < cutplane_value) {
	    // Ray is crossing the plane from a lower value
	    intersection1 = lower->intersect(ray,min_t,intersect_t);
	    intersection2 = higher->intersect(ray,intersect_t,max_t);
	} else {
	    // Ray is crossing the plane from a higher value
	    intersection1 = higher->intersect(ray,min_t,intersect_t);
	    intersection2 = lower->intersect(ray,intersect_t,max_t);
	}
	
	if (intersection1.isIntersected() && intersection2.isIntersected()) {
	    if (intersection1.getT() < intersection2.getT()) {
		return intersection1;
	    } else {
		return intersection2;
	    }
	} else {
	    if (intersection1.isIntersected()) {
		return intersection1;
	    } else {
		return intersection2;
	    }
	}
     }
}

const Intersection BSP::intersectForShadow_recurse(const Ray& ray, const double min_t, const double max_t) const {
    if (max_t <= min_t) return Intersection();

    //Vector o = ray.getOrigin() + min_t * ray.getDirection();
    double rd_dim = ray.getDirection()[cutplane_dimension];
    double o_dim = ray.getOrigin()[cutplane_dimension] + min_t * rd_dim;
    
    if (o_dim < cutplane_value && rd_dim <= 0) {
        return lower->intersectForShadow(ray,min_t,max_t);
    } else if (o_dim > cutplane_value && rd_dim >= 0) {
        return higher->intersectForShadow(ray,min_t,max_t);
    } else {
	double intersect_t = (cutplane_value - ray.getOrigin()[cutplane_dimension]) * ray.getInverseDirection()[cutplane_dimension];
	if (intersect_t > max_t) { intersect_t = max_t; }
	if (intersect_t < min_t) { intersect_t = min_t; }
	
	Intersection intersection1;

	if (o_dim < cutplane_value) {
	    // Ray is crossing the plane from a lower value
	    intersection1 = lower->intersectForShadow(ray,min_t,intersect_t);
	    if (intersection1.isIntersected()) {
		return intersection1;
	    } else {
		return higher->intersectForShadow(ray,intersect_t,max_t);
	    }
	} else {
	    // Ray is crossing the plane from a higher value
	    intersection1 = higher->intersectForShadow(ray,min_t,intersect_t);
	    if (intersection1.isIntersected()) {
		return intersection1;
	    } else {
		return lower->intersectForShadow(ray,intersect_t,max_t);
	    }
	}
    }
}

BoundingBox BSP::enclosure() const {
    assert(objects.size() > 0);
    BoundingBox result = objects[0]->boundingBoundingBox(); 
    for(unsigned int i = 1; i < objects.size(); i++) {
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
    } else {
	return -1;
       // Throw an exception
    }
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

    // Test enclosure()
    BoundingBox box = bsp.enclosure();
    assert(IS_EQUAL(box.minimum()[0],-210));
    assert(IS_EQUAL(box.minimum()[1],-160));
    assert(IS_EQUAL(box.minimum()[2],-210));
    assert(IS_EQUAL(box.maximum()[0],210));
    assert(IS_EQUAL(box.maximum()[1],260));
    assert(IS_EQUAL(box.maximum()[2],210));

    // Test largestDimension()
    bsp.addObject(new Sphere(Vector(0,-500,0),10,Material(RGB(0.8,0.8,0.8),0.7,RGB(1.0,1.0,1.0),0.80,40)));
    assert(largestDimension(bsp.enclosure()) == 1);

    bsp.prepare();
    // Test intersection
    Ray r = Ray(Vector(200,250,1000),Vector(0,0,-1),1);
    assert(bsp.intersect(r).isIntersected());
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[0],200));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[1],250));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[2],210));

    r = Ray(Vector(200,250,-1000),Vector(0,0,1),1);
    assert(bsp.intersect(r).isIntersected());
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[0],200));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[1],250));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[2],-210));

    r = Ray(Vector(-200,-150,1000),Vector(0,0,-1),1);
    assert(bsp.intersect(r).isIntersected());
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[0],-200));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[1],-150));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[2],210));

    r = Ray(Vector(0,1000,0),Vector(0,-1,0),1);
    assert(bsp.intersect(r).isIntersected());
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[0],0));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[1],260));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[2],0));

    r = Ray(Vector(0,-1000,0),Vector(0,1,0),1);
    assert(bsp.intersect(r).isIntersected());
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[0],0));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[1],-510));
    assert(IS_EQUAL(bsp.intersect(r).getPoint()[2],0));

    r = Ray(Vector(300,250,-1000),Vector(0,0,1),1);
    assert(!bsp.intersect(r).isIntersected());

    r = Ray(Vector(200,250,-1000),Vector(0,0,-1),1);
    assert(!bsp.intersect(r).isIntersected());

    std::cout << "BSP::test() done." << std::endl;
}
