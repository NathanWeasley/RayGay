
#include <iostream>
#include <vector>
#include <cassert>
#include <map>

#include "mesh.h"
#include "math/matrix.h"
#include "math/vector2.h"
#include "intersection.h"
#include "ray.h"
#include "triangle.h"
#include "paths/circle.h"
#include "extrusion.h"
#include "paths/linesegment.h"
#include "space/kdtree.h"

#define PHONG_ANGLETHRESHOLD 0.4226f // Smoothing threshold approx. 65 Deegrees :) 

using namespace std;

// ----------------------------------------------------------------------------
Mesh::Mesh(MeshType type, const Material* mat) {
    meshType = type;
    prepared = false;
    material = mat;
}

/*
Mesh::Mesh() : SceneObject(NULL) {
    prepared = false;
}
*/

// ----------------------------------------------------------------------------
Mesh::~Mesh() {
    for(unsigned int i = 0; i < triangles.size(); i++) {
	delete triangles[i];
    }
}

void Mesh::addSelf(KdTree* space) {
    for(unsigned int i = 0; i < triangles.size(); i++) {
	space->addObject(triangles[i]);
    }
}

void Mesh::prepare() {
    if (prepared == true) return;

    computeAdjacentTris();
    computeInterpolatedNormals();
    prepared = true;
    for(unsigned int i = 0; i < triangles.size(); i++) {
	triangles[i]->prepare();
    }
}

// ----------------------------------------------------------------------------
/**
 * Add a triangle to this mesh
 * @param c must a pointer to three Vectors
 * @param uv must a pointer to three Vector2s
 */
void Mesh::addTriangle(const Vector* c, const Vector2* uv) {
    Vector normal = Vector::xProduct(c[1] - c[0], c[2] - c[0]);
    normal.normalize();
    normals.push_back(normal);
    Triangle* t = new Triangle(this);
    t->normali = normals.size() - 1;

    for(int i = 0; i < 3; i++) {
	int new_index = findExistingCorner(&c[i]);
	if (new_index == -1) {
	   corners.push_back(c[i]);
	   new_index = corners.size() - 1;
	   vertices.push_back(Vertex(new_index));
	}
        t->vertex[i] = new_index;
    }

    Tri* tri = new Tri(t->vertex[0],t->vertex[1],t->vertex[2]);
    tris.push_back(tri);
    tri->normal_idx = t->normali;

    triangles.push_back(t);
    t->setTri(triangles.size() - 1);

    tri->uv[0] = uv[0];
    tri->uv[1] = uv[1];
    tri->uv[2] = uv[2];

    // Insert edges
    for(int i = 0; i < 3; i++) {
	int j = (i + 1) % 3;
	EdgeKey key = EdgeKey(t->vertex[i],t->vertex[j]);
	Edge* edge = edgeMap[key];
	if (edge == NULL) {
	    edge = new Edge(t->vertex[i],t->vertex[j]);
	    edge->triangle[0] = tri;
	    edgeMap[key] = edge;
	} else {
	    edge->triangle[1] = tri;
	}
	tri->edge[i] = edge;
    }
    
    // Add to tris-list at each vertex
    for(int i = 0; i < 3; i++) {
	vertices[t->vertex[i]].tris.push_back(tri);
    }
}

/**
 * Add a quad to the mesh. This results in two triangles being added.
 *
 * @param corners A pointer to four vertices in clockwise direction
 * @param uv A pointer to four uv coordinates
 */
void Mesh::addQuad(const Vector* corners, const Vector2* uv) {
    addTriangle(corners[0],corners[1],corners[2],
	        uv[0],uv[1],uv[2]);
    addTriangle(corners[0],corners[2],corners[3],
	        uv[0],uv[2],uv[3]);
}

void Mesh::addVertex(const Vector& point) {
    corners.push_back(point);
    int new_index = corners.size() -1;
    vertices.push_back(Vertex(new_index));
}

void Mesh::addTriangle(int v[3]) {
    Vector2 uv[3];
    addTriangle(v,uv);
}

void Mesh::addTriangle(int v[3], Vector2 uv[3]) {
    Vector c[3];
    c[0] = cornerAt(v[0]);
    c[1] = cornerAt(v[1]);
    c[2] = cornerAt(v[2]);
    Vector normal = Vector::xProduct(c[1] - c[0], c[2] - c[0]);
    normal.normalize();
    normals.push_back(normal);
    Triangle* t = new Triangle(this);
    t->normali = normals.size() - 1;
    t->vertex[0] = v[0];
    t->vertex[1] = v[1];
    t->vertex[2] = v[2];

    Tri* tri = new Tri(t->vertex[0],t->vertex[1],t->vertex[2]);
    tris.push_back(tri);
    tri->normal_idx = t->normali;

    triangles.push_back(t);
    t->setTri(triangles.size() - 1);

    tri->uv[0] = uv[0];
    tri->uv[1] = uv[1];
    tri->uv[2] = uv[2];

    // Insert edges
    for(int i = 0; i < 3; i++) {
	int j = (i + 1) % 3;
	EdgeKey key = EdgeKey(t->vertex[i],t->vertex[j]);
	Edge* edge = edgeMap[key];
	if (edge == NULL) {
	    edge = new Edge(t->vertex[i],t->vertex[j]);
	    edge->triangle[0] = tri;
	    edgeMap[key] = edge;
	} else {
	    edge->triangle[1] = tri;
	}
	tri->edge[i] = edge;
    }
    
    // Add to tris-list at each vertex
    for(int i = 0; i < 3; i++) {
	vertices[t->vertex[i]].tris.push_back(tri);
    }

}

void Mesh::computeAdjacentTris() {
    for(unsigned int i = 0; i < tris.size(); i++) {
	Tri* tri = tris[i];
	for(unsigned int e = 0; e < 3; e++) {
	    Tri* adj;
	    Edge* edge = tri->edge[e];
	    if (edge->triangle[0] == tri) {
		adj = edge->triangle[1];
	    } else {
		adj = edge->triangle[0];
	    }
	    tri->triangle[e] = adj;
	}
    }
}

void Mesh::computeInterpolatedNormals() {
    for(unsigned int i = 0; i < tris.size(); i++) {
	Tri* tri = tris[i];
	Vector normal = normals[tri->normal_idx];
	for(unsigned int j = 0; j < 3; j++) {
	    Vertex vertex = vertices[tri->vertex[j]];
	    int num = 1;
	    Vector interpolated_normal = normal;
	    for(unsigned int v = 0; v < vertex.tris.size(); v++) {
		Tri* other_tri = vertex.tris[v];
		Vector other_normal = normals[other_tri->normal_idx];
		if (other_tri != tri &&
			other_normal * normal > PHONG_ANGLETHRESHOLD) {
		    interpolated_normal = interpolated_normal + other_normal;
		    num++;
		}
	    }
	    if (num > 1) {
		interpolated_normal.normalize();
		normals.push_back(interpolated_normal);
		tri->interpolated_normal[j] = normals.size() -1;
	    } else {
		tri->interpolated_normal[j] = tri->normal_idx;
	    }
	}
    }
}

int Mesh::findExistingCorner(const Vector* c) const {
    unsigned int size = corners.size();
    for(unsigned int i = 0; i < size; i++) {
	if (corners[i] == *c) return i;
    }
    return -1;
}

// ----------------------------------------------------------------------------
/**
 * Add a triangle to this mesh
 */
void Mesh::addTriangle(const Vector& c1, const Vector& c2, const Vector& c3,
	const Vector2& uv1, const Vector2& uv2,const Vector2& uv3) {
    Vector c[3];
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
    Vector2 uv[3];
    uv[0] = uv1;
    uv[1] = uv2;
    uv[2] = uv3;
    addTriangle(c,uv);
}

/**
 * Add a triangle to this mesh
 */
void Mesh::addTriangle(const Vector& c1, const Vector& c2, const Vector& c3) {
    Vector c[3];
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
    Vector2 uv[3];
    addTriangle(c,uv);
}


// ----------------------------------------------------------------------------
void Mesh::transform(const Matrix& M) {
    for (vector<Vector>::iterator p = corners.begin(); p != corners.end(); p++) {
	(*p) = M * (*p);
    }
    Matrix rot = M.extractRotation();
    for (vector<Vector>::iterator p = normals.begin(); p != normals.end(); p++) {
	(*p) =  rot * (*p);
    }
}

// ----------------------------------------------------------------------------
Vector Mesh::normal(const Triangle* const triangle, double u, double v) const {
    return phong_normal(triangle,u,v);
 //      return normals[triangle->normali]; // Flat
}

Vector Mesh::phong_normal(const Triangle* const triangle, double u, double v) const {
    //const Triangle* triangle = (Triangle*) i.getLocalObject();
    Tri* tri = tris[triangle->getTri()];
    Vector result = Vector(0,0,0);
    Vector weight = Vector(1-u-v,u,v);
    for(unsigned int j = 0; j < 3; j++) {
	result += normals[tri->interpolated_normal[j]] * weight[j];
    }
    result.normalize();
    return result;
}

const Material* Mesh::getMaterial() const {
    return material;
}

Vector2 Mesh::getUV(const Triangle* triangle, double u, double v) const {
    Tri* tri = tris[triangle->getTri()];
    return tri->uv[0] * (1-u-v) +
           tri->uv[1] * u +
           tri->uv[2] * v;
}

std::vector<Vector>* Mesh::getVertices() {
    std::vector<Vector>* result = new std::vector<Vector>;
    for(unsigned int i = 0; i < corners.size(); i++) {
	result->push_back(corners[i]);
    }
    return result;
}

/**
 * Returns a vector of all unique edges in the mesh. 
 * An edge is represented as a Linesegment.
 */
std::vector<Linesegment>* Mesh::getEdges() {
    
    std::vector<Linesegment>* result = new std::vector<Linesegment>;
    for(EdgeMapType::iterator h = edgeMap.begin(); h != edgeMap.end(); h++) {
	Edge* edge = h->second;
	Linesegment s = Linesegment(corners[edge->vertex[0]],corners[edge->vertex[1]]);
	result->push_back(s);
    }
    return result;
}

SceneObject* Mesh::clone() const {
    Mesh* clone = new Mesh(meshType,material);
    // Copy triangles and that's it.
    unsigned int num = triangles.size();
    Vector vs[3];
    Vector2 uvs[3];
    for (unsigned int i = 0; i < num; i++) {
	Tri* tri = tris[i];
	for (unsigned int j = 0; j < 3; j++) {
	    vs[j] = cornerAt(tri->vertex[j]);
	    uvs[j] = tri->uv[j];
	}
	clone->addTriangle(vs,uvs);
    }
    return clone;
}

// ----------------------------------------------------------------------------
Mesh::Edge::Edge(int iV0, int iV1) {
    vertex[0] = iV0;
    vertex[1] = iV1;
    triangle[0] = NULL;
    triangle[1] = NULL;
}

// ----------------------------------------------------------------------------
Mesh::Tri::Tri(int iV0, int iV1, int iV2) {
    vertex[0] = iV0;
    vertex[1] = iV1;
    vertex[2] = iV2;
    normal_idx = -1;
    for(unsigned int i = 0; i < 3; i++) {
	interpolated_normal[i] = -1;
    }
}

Mesh::Vertex::Vertex(int iV) {
    index = iV;
}

// ----------------------------------------------------------------------------
void Mesh::test() {
#ifdef GAHAGAH
    KdTree bsp = KdTree();

    Material* mat = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Mesh mesh = Mesh(MESH_FLAT,mat);
    mesh.addTriangle(Vector(-1,1,1),Vector(1,1,1),Vector(0,-1,-1));
    mesh.prepare();
    mesh.addSelf(&bsp);
    bsp.prepare();

    assert(mesh.edgeMap.size() == 3);

    // Test intersection
    Ray ray = Ray(Vector(0,0,100),Vector(0,0,-1),0.0);
    assert(bsp.intersect(ray));
    Intersection* i = bsp.getLastIntersection();
    assert(i->getPoint() == Vector(0,0,0));
    assert(i->isIntersected());
    assert(i->getT() == 100.0);

    ray = Ray(Vector(0,0,100),Vector(0,0,1),0.0);
    assert(bsp.intersect(ray) == false);

    ray = Ray(Vector(0,0,-100),Vector(0,0,-1),0.0);
    assert(bsp.intersect(ray) == false);

    // Test torus
    Circle circle1 = Circle(Vector(0,75,0),200,Vector(0,1,0));
    Extrusion torus = Extrusion(circle1,100,16,10,Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.20,30));
    torus.prepare();

    assert(torus.corners.size() == 16*10);
    assert(torus.vertices.size() == 16*10);

    for(unsigned int i = 0; i < torus.tris.size(); i++) {
	Tri* tri = torus.tris[i];
	assert(tri->normal_idx != -1);
	for(unsigned int j = 0; j < 3; j++) {
	   assert(tri->interpolated_normal[j] != -1);
	   assert(tri->vertex[j] != -1);
	   for (unsigned int k = 0; k < 3; k++) {
	       if (k != j) {
		   assert(tri->interpolated_normal[k] != tri->interpolated_normal[j]);
	       }
	   }
	}
    }

    for(unsigned int i = 0; i < torus.vertices.size(); i++) {
	Vertex v = torus.vertices[i];
	assert(v.tris.size() == 6);
    }

    assert(torus.normals.size() == 4 * torus.tris.size());
   
    for(unsigned int i = 0; i < torus.normals.size(); i++) {
	Vector normal = torus.normals[i];
	assert(IS_EQUAL(normal.norm(),double(1.0)));
    }

    delete mat;

    cout << "Mesh::test() done." << endl;
#endif
}
