#ifndef CYLINDER_H
#define CYLINDER_H

#include "mesh.h"

class Vector;
class Path;

/// A cylinder object
class Cylinder : public Mesh {

    public:
	/// Constructor
    	Cylinder(const Vector& begin, const Vector& end, double radius, unsigned int segments, Material m);

        Cylinder(const Path& path, double radius, unsigned int segments, unsigned int pieces, Material m);
};

#endif
