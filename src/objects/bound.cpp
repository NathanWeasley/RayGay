
#include "space/kdtree.h"
#include "boundingbox.h"
#include "objects/bound.h"
#include "objects/objectgroup.h"
#include "math/vector2.h"
#include "materials/plastic.h"

Bound::Bound(ObjectGroup* group) : Object(new Plastic(RGB(1.0,1.0,1.0))) {
    this->group = group;
    tree = new KdTree();
    group->prepare();
    group->addSelf(tree);
    tree->prepare();
    running = false;
}

void Bound::prepare() {
}

void Bound::transform(const Matrix& m) {
    group->transform(m);
}

BoundingBox Bound::boundingBoundingBox() const {
    return tree->boundingBox();
}

Vector Bound::normal(const Intersection &i) const {
    // Not to be called
    return Vector(1,0,0);
}

Vector2 Bound::getUV(const Intersection& i) const {
    // Not to be called
    return Vector2(-1,-1);
}

SceneObject* Bound::clone() const {
    // Not to be called
    return NULL;
}

/*
const Material* Bound::getMaterial() const {
    if (last_intersection.isIntersected()) {
	return last_intersection.getObject()->getMaterial();
    } else {
	return NULL;
    }
}
*/

bool Bound::intersect(const Ray& ray) const {
    if (tree->intersect(ray)) {
	last_intersection = *(tree->getLastIntersection());
    } else {
	last_intersection = Intersection();
    } 
    return last_intersection.isIntersected();
}

Intersection Bound::_intersect(const Ray& ray) const {
    // Not to be called
    return Intersection();
}
