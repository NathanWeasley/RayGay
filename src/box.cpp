
#include "box.h"

#include <iostream>
#include <cassert>

#include "vector.h"
#include "ray.h"
#include "intersection.h"
#include "matrix.h"
#include "rgb.h"
#include "material.h"
#include "constants.h"
#include "mesh.h"

Box::Box(const Vector a, const Vector b, Material mat) : Mesh(Mesh::MESH_FLAT,mat) {
    
    Vector c2 = Vector(min(a[0],b[0]),min(a[1],b[1]),min(a[2],b[2]));
    Vector c7 = Vector(max(a[0],b[0]),max(a[1],b[1]),max(a[2],b[2]));
    
    Vector c1 = Vector(c7[0],c2[1],c2[2]);
    Vector c4 = Vector(c2[0],c7[1],c2[2]);
    Vector c3 = Vector(c7[0],c7[1],c2[2]);

    Vector c5 = Vector(c7[0],c2[1],c7[2]);
    Vector c6 = Vector(c2[0],c2[1],c7[2]);
    Vector c8 = Vector(c2[0],c7[1],c7[2]);
    /*
    //mesh = new Mesh(Mesh::MESH_FLAT,mat);
    mesh->addTriangle(c1,c2,c3); mesh->addTriangle(c2,c4,c3);
    mesh->addTriangle(c2,c6,c4); mesh->addTriangle(c6,c8,c4);
    mesh->addTriangle(c5,c6,c2); mesh->addTriangle(c1,c5,c2);
    mesh->addTriangle(c6,c5,c7); mesh->addTriangle(c6,c7,c8);
    mesh->addTriangle(c5,c1,c7); mesh->addTriangle(c1,c3,c7);
    mesh->addTriangle(c3,c4,c7); mesh->addTriangle(c4,c8,c7);
    */
    Mesh::addTriangle(c1,c2,c3); Mesh::addTriangle(c2,c4,c3);
    Mesh::addTriangle(c2,c6,c4); Mesh::addTriangle(c6,c8,c4);
    Mesh::addTriangle(c5,c6,c2); Mesh::addTriangle(c1,c5,c2);
    Mesh::addTriangle(c6,c5,c7); Mesh::addTriangle(c6,c7,c8);
    Mesh::addTriangle(c5,c1,c7); Mesh::addTriangle(c1,c3,c7);
    Mesh::addTriangle(c3,c4,c7); Mesh::addTriangle(c4,c8,c7);
}

Box::Box(const Vector c, double width, double height, double depth, Material m): Mesh(Mesh::MESH_FLAT,m) {
    width /= 2;
    height /= 2;
    depth /= 2;
    Vector c1 = Vector(c[0]-width,c[1]-height,c[2]-depth);
    Vector c2 = Vector(c[0]+width,c[1]+height,c[2]+depth);
    Box(c1,c2,m);
}

Box::~Box() {
    // Do we leak stuff inside Mesh here?
}

