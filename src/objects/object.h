
#ifndef OBJECT_H
#define OBJECT_H

#include <string>

#include "intersection.h"
#include "sceneobject.h"

class Matrix;
class Ray;
class Vector;
class Vector2;
class BoundingBox;
class Material;
class SpaceSubdivider;

/// The abstract superclass of all objects in the scene that can be rendered.

class Object : public SceneObject {
    public:
        /// Return the nearest intersection to ray's origin
	virtual bool intersect(const Ray& ray) const;

	/// Returns the normalvector at a point on this objects surface
	virtual Vector normal(const Intersection &i) const = 0;
	
	/// Transform this object
	virtual void transform(const Matrix& m) = 0;

	/// Returns the materiale of this object
	virtual const Material* getMaterial() const { 
	    if (last_intersection.isIntersected()) {
		return last_intersection.getObject()->material;
	    } else {
		return material;
	    }
	};

	/// The smallest box containing this object
	virtual BoundingBox boundingBoundingBox() const = 0;
	
	/// Get texture coordinates at an intersection
	virtual Vector2 getUV(const Intersection& intersection) const = 0;

	/// Prepares the object before rendering
	virtual void prepare();

	void addSelf(SpaceSubdivider* space);

	/// Returns last successful intersection
	Intersection* getLastIntersection() const { return &last_intersection; }; 

    protected:
	Object(const Material* material);
	/// Internal intersect method that subclasses must implement
	virtual Intersection _intersect(const Ray& ray) const = 0;
	mutable Intersection last_intersection;

    private:	
	// Two members for caching last intersection
	mutable long last_ray;
	const Material* material;
};

/**
 *  A caching proxy around the private _intersect(Ray) method in subclasses.
 *  Because an object can exist in several bounding boxes we end up shooting
 *  the same ray at the same object several times.
 */
inline
bool Object::intersect(const Ray& ray) const {
    if (ray.getId() != last_ray) {
	last_intersection = _intersect(ray);
	last_intersection.setObject(const_cast<Object*>(this));
	last_ray = ray.getId();
    }
    return last_intersection.isIntersected();
}


#endif
