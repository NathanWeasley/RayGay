#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <cassert>
#include <iostream>

#include "boundingbox.h"
#include "objects/sphere.h"
#include "objects/cylinder.h"
#include "objects/boolean.h"
#include "objects/mesh.h"
#include "objects/extrusion.h"
#include "objects/box.h"
#include "objects/necklace.h"
#include "objects/transformedinstance.h"
#include "objects/torus.h"
#include "objects/3ds.h"
#include "objects/tetrahedron.h"
#include "objects/tessalation.h"
#include "objects/booleanoperand.h"
#include "space/kdtree.h"
#include "paths/linesegment.h"
#include "paths/circle.h"

bool intersects(Object* o, const Ray& ray) {
    return o->fastIntersect(ray) > 0;
}
bool intersects(Object* o, const Vector& origin, const Vector& dir) {
    Ray ray = Ray(origin,dir,1);
    return intersects(o,ray);
}

Vector iPoint(Object* o, const Ray& ray) {
    double t = o->fastIntersect(ray);
    assert(t > 0);
    return o->fullIntersect(ray,t).getPoint();
}

Vector iPoint(Object* o, const Vector& origin, const Vector& dir) {
    Ray ray = Ray(origin,dir,1);
    return iPoint(o,ray);
}

Vector iNormal(Object* o, const Ray& ray) {
    double t = o->fastIntersect(ray);
    assert(t > 0);
    Intersection i = o->fullIntersect(ray,t);
    return i.getObject()->normal(i);
}

Vector iNormal(Object* o, const Vector& origin, const Vector& dir) {
    Ray ray = Ray(origin,dir,1);
    return iNormal(o,ray);
}


void sphere_test() {
    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Sphere s = Sphere(Vector(0,0,0),10.0,m);
    assert(s.inside(Vector(0,0,0)));
    assert(s.inside(Vector(9,0,0)));
    assert(!s.inside(Vector(10,0,0)));
    assert(!s.inside(Vector(11,0,0)));
    assert(s.onEdge(Vector(10,0,0)));
    assert(s.onEdge(Vector(0,10,0)));
    assert(!s.onEdge(Vector(0,0,0)));

    s = Sphere(Vector(0,0,0),60.0,m);

    /* Test intersection(ray) */
    //assert(intersects(&s,Vector(0,0,1000),Vector(0,0,-1)));
    //assert(IS_EQUAL(s.getLastIntersection()->getPoint()[2],60.0));
    
    assert(intersects(&s,Vector(0,0,1000),Vector(0,0,-1)));
    assert(IS_EQUAL(iPoint(&s,Vector(0,0,1000),Vector(0,0,-1))[2],60.0));

    //r = Ray(Vector(0,0,0),Vector(0,0,-1),1);
   // assert(s.intersect(r));
 //   assert(IS_EQUAL( s.getLastIntersection()->getPoint()[2], -60.0));

    assert(intersects(&s,Vector(0,0,0),Vector(0,0,-1)));
    assert(IS_EQUAL(iPoint(&s,Vector(0,0,0),Vector(0,0,-1))[2],-60.0));

    assert(intersects(&s,Vector(0,0,-1000),Vector(0,0,1)));
    assert(IS_EQUAL(iPoint(&s,Vector(0,0,-1000),Vector(0,0,1))[2],-60.0));

    assert(intersects(&s,Vector(0,0,-100),Vector(0,0,-1)) == false);

    assert(intersects(&s,Vector(0,0,-60),Vector(0,0,-1)) == false);

    /* Test boundingBoundingBox() */
    s = Sphere(Vector(0,0,0),20.0,m);
    assert(s.boundingBoundingBox() == BoundingBox(Vector(-20,-20,-20),Vector(20,20,20)));

    /* Test clone */
    Object* s1 = new Sphere(Vector(0,0,0),20,m);
    Object* s2 = dynamic_cast<Object*>(s1->clone());
    s1->transform(Matrix::matrixTranslate(Vector(0,-100,0)));
    s2->transform(Matrix::matrixTranslate(Vector(0,100,0)));

    //r = Ray(Vector(0,-100,-1000),Vector(0,0,1),1);
    //assert(s1->intersect(r));
    //assert(!s2->intersect(r));
    assert(intersects(s1,Vector(0,-100,-1000),Vector(0,0,1)) == true);
    assert(intersects(s2,Vector(0,-100,-1000),Vector(0,0,1)) == false);
    //r = Ray(Vector(0,100,-1000),Vector(0,0,1),1);
    //assert(!s1->intersect(r));
    //assert(s2->intersect(r));
    assert(intersects(s1,Vector(0,100,-1000),Vector(0,0,1)) == false);
    assert(intersects(s2,Vector(0,100,-1000),Vector(0,0,1)) == true);
}

void boolean_test() {
    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Sphere* s1 = new Sphere(Vector(-10,0,0),30.0,m);
    Sphere* s2 = new Sphere(Vector(10,0,0),30.0,m);

    /* Test BOOLEAN_UNION */
    Boolean* b = new Boolean(s1,Boolean::BOOLEAN_UNION,s2,m);
    assert(b->inside(Vector(0,0,0)));
    assert(b->inside(Vector(39,0,0)));
    assert(b->onEdge(Vector(40,0,0)));
    assert(!b->inside(Vector(40,0,0)));
    assert(b->inside(Vector(-20,0,0)));

    assert(b->onEdge(Vector(-40,0,0)));
    assert(b->onEdge(Vector(10,30,0)));

    assert(!b->onEdge(Vector(-20,0,0)));
    assert(!b->onEdge(Vector(20,0,0)));
    assert(!b->onEdge(Vector(0,0,0)));

    /* Test BOOLEAN_INTERSECTION */
    b = new Boolean(s1,Boolean::BOOLEAN_INTERSECTION,s2,m);
    assert(b->inside(Vector(0,0,0)));
    assert(b->inside(Vector(19,0,0)));
    assert(b->inside(Vector(-19,0,0)));
    assert(!b->inside(Vector(39,0,0)));
    assert(!b->inside(Vector(-39,0,0)));
    assert(!b->inside(Vector(20,0,0)));
    assert(!b->inside(Vector(-20,0,0)));

    assert(!b->onEdge(Vector(-40,0,0)));
    assert(!b->onEdge(Vector(40,0,0)));
    assert(!b->onEdge(Vector(0,0,0)));
    assert(b->onEdge(Vector(-20,0,0)));
    assert(b->onEdge(Vector(20,0,0)));
    
    /* Test BOOLEAN_DIFFERENCE */
    b = new Boolean(s1,Boolean::BOOLEAN_DIFFERENCE,s2,m);
    assert(!b->inside(Vector(-40,0,0)));
    assert(b->inside(Vector(-39,0,0)));
    assert(b->inside(Vector(-21,0,0)));
    assert(!b->inside(Vector(-20,0,0)));
    assert(!b->inside(Vector(0,0,0)));
    assert(!b->inside(Vector(40,0,0)));
    assert(!b->inside(Vector(39,0,0)));

    assert(!b->onEdge(Vector(-41,0,0)));
    assert(b->onEdge(Vector(-40,0,0)));
    assert(!b->onEdge(Vector(-39,0,0)));
    assert(!b->onEdge(Vector(-21,0,0)));
    assert(b->onEdge(Vector(-20,0,0)));
    assert(!b->onEdge(Vector(-19,0,0)));
    assert(!b->onEdge(Vector(0,0,0)));
    assert(!b->onEdge(Vector(20,0,0)));
    assert(!b->onEdge(Vector(40,0,0)));

    Sphere* s3 = new Sphere(Vector(-10,0,0),10.0,m);
    Sphere* s4 = new Sphere(Vector(10,0,0),10.0,m);
    b = new Boolean(s3,Boolean::BOOLEAN_INTERSECTION,s4,m);
    assert(b->onEdge(Vector(0,0,0)));

    /* Intersection test */
    s1 = new Sphere(Vector(0,0,0),60.0,m);
    s2 = new Sphere(Vector(0,0,60),40.0,m);

    b = new Boolean(s1,Boolean::BOOLEAN_DIFFERENCE,s2,m);

    /*
    Ray r = Ray(Vector(0,0,1000),Vector(0,0,-1),1);
    assert(b->intersect(r));
    Intersection* i = b->getLastIntersection();
    assert(i->getPoint() == Vector(0,0,20));
    assert(b->normal(*i) == Vector(0,0,1));
    */
    
    assert(intersects(b,Vector(0,0,1000),Vector(0,0,-1)));
    assert(iPoint(b,Vector(0,0,1000),Vector(0,0,-1)) == Vector(0,0,20));
    assert(iNormal(b,Vector(0,0,1000),Vector(0,0,-1)) == Vector(0,0,1));

    /*
    r = Ray(Vector(0,0,-1000),Vector(0,0,1),1);
    assert(b->intersect(r));
    i = b->getLastIntersection();
    assert(i->getPoint() == Vector(0,0,-60));
    assert(b->normal(*i) == Vector(0,0,-1));
    */
    assert(intersects(b,Vector(0,0,-1000),Vector(0,0,1)));
    assert(iPoint(b,Vector(0,0,-1000),Vector(0,0,1)) == Vector(0,0,-60));
    assert(iNormal(b,Vector(0,0,-1000),Vector(0,0,1)) == Vector(0,0,-1));

    // Test a sphere with three other spheres subtracted from its middle,
    // front and back, so that the resulting object is hollow along the z-axis.

    s1 = new Sphere(Vector(0,0,0),200.0,m);
    s2 = new Sphere(Vector(0,0,0),180.0,m);
    Boolean* s = new Boolean(s1,Boolean::BOOLEAN_DIFFERENCE,s2,m); // Make it hollow
    assert(!s->inside(Vector(0,0,0)));
    assert(!s->onEdge(Vector(0,0,0)));
    assert(s->inside(Vector(0,0,190)));
    assert(!s->onEdge(Vector(0,0,190)));
    assert(s->onEdge(Vector(0,0,180)));
    assert(!s->inside(Vector(0,0,180)));
    assert(s->onEdge(Vector(0,0,200)));
    assert(!s->inside(Vector(0,0,200)));
    assert(s->inside(Vector(0,0,-190)));
    assert(s->inside(Vector(0,190,0)));
    assert(s->inside(Vector(190,0,0)));

    s3 = new Sphere(Vector(0,0,200),100.0,m); // Cut front
    Boolean* b4 = new Boolean(s,Boolean::BOOLEAN_DIFFERENCE,s3,m);
    assert(s->inside(Vector(0,0,190)));
    assert(s->inside(Vector(0,0,-190)));
    assert(!b4->inside(Vector(0,0,190)));
    assert(!b4->onEdge(Vector(0,0,190)));
    assert(b4->inside(Vector(0,0,-190)));
    assert(b4->inside(Vector(0,190,0)));
    
    s4 = new Sphere(Vector(0,0,-200),100.0,m); // Cut back
    Boolean* b5 = new Boolean(b4,Boolean::BOOLEAN_DIFFERENCE,s4,m);
    assert(!b5->inside(Vector(0,0,190)));
    assert(!b5->onEdge(Vector(0,0,190)));
    assert(!b5->inside(Vector(0,0,-190)));
    assert(!b5->inside(Vector(0,0,-250)));
    assert(!b5->onEdge(Vector(0,0,-190)));
    assert(!b5->onEdge(Vector(0,0,-200)));
    assert(!b5->onEdge(Vector(0,0,-300)));
    assert(b5->inside(Vector(0,190,0)));
    assert(b5->inside(Vector(0,-190,0)));
    assert(b5->onEdge(Vector(0,200,0)));
    assert(b5->onEdge(Vector(0,-200,0)));
    assert(!b5->inside(Vector(0,0,0)));
    assert(!b5->onEdge(Vector(0,0,0)));
    
    Ray r = Ray(Vector(0,0,1000),Vector(0,0,-1),1);
    //assert(s3->intersect(r));
    //assert(s4->intersect(r));
    //assert(b4->intersect(r));
    assert(intersects(s,r));
    assert(intersects(s3,r));
    assert(intersects(s4,r));
    assert(intersects(b4,r));
    r = Ray(Vector(0,0,-1000),Vector(0,0,1),1);
    assert(! intersects(b5,r));
 /*   i = b5->getLastIntersection();
    cout << i->getPoint() << "  ...." << endl;
    assert(!b5->inside(Vector(0,0,0)));
    assert(!b5->onEdge(Vector(0,0,0)));*/
}

void box_test() {
    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Box* b = new Box(Vector(-1,-1,-1),Vector(1,1,1),m);
    b->prepare();
    assert(b->getVertices()->size() == 8);

    KdTree* bsp = new KdTree();
    b->addSelf(bsp);
    bsp->prepare();
    Intersection* inter = new Intersection();
    Ray r = Ray(Vector(0,0,100),Vector(0,0,-1),1);
    assert(bsp->intersect(r,inter));
    assert(inter->getPoint() == Vector(0,0,1));
    assert(inter->getObject()->getUV(*inter) == Vector2(0.5,0.5));

    r = Ray(Vector(0,-100,0),Vector(0,1,0),1);
    assert(bsp->intersect(r,inter));
    assert(inter->getPoint() == Vector(0,-1,0));

    /* Test second constructor */
    b = new Box(Vector(0,0,0),2,2,2,m);
    b->prepare();
    assert(b->getVertices()->size() == 8);

    bsp = new KdTree();
    b->addSelf(bsp);
    bsp->prepare();

    r = Ray(Vector(0,0,100),Vector(0,0,-1),1);
    assert(bsp->intersect(r,inter));
    assert(inter->getPoint() == Vector(0,0,1));

    r = Ray(Vector(0,-100,0),Vector(0,1,0),1);
    assert(bsp->intersect(r,inter));
    assert(inter->getPoint() == Vector(0,-1,0));

    r = Ray(Vector(0,-100,1.5),Vector(0,1,0),1);
    assert(bsp->intersect(r,inter) == false);

    /* test clone() */
    b = new Box(Vector(-1,-1,-1),Vector(1,1,1),m);
    SceneObject* b2 = b->clone();
    assert(b2 != NULL);
    b->transform(Matrix::matrixTranslate(Vector(0,10,0)));
    b->prepare();
    b2->prepare();
    bsp = new KdTree();
    b->addSelf(bsp);
    b2->addSelf(bsp);
    bsp->prepare();

    Intersection i;
    r = Ray(Vector(0,0,100),Vector(0,0,-1),1);
    assert(bsp->intersect(r,&i));
    r = Ray(Vector(0,10,100),Vector(0,0,-1),1);
    assert(bsp->intersect(r,&i));
    r = Ray(Vector(0,5,100),Vector(0,0,-1),1);
    assert(!bsp->intersect(r,&i));
}

void mesh_test() {
}

void tetrahedron_test() {
    Material* mat = new Material(RGB(.0,.0,.0),RGB(.0,.0,.0));
    Tetrahedron t = Tetrahedron(Vector(0,0,0),100,mat);
    assert(t.getEdges()->size() == 6);
    assert(t.getVertices()->size() == 4);
}

void tesselation_test() {
    Material* mat = new Material(RGB(.0,.0,.0),RGB(.0,.0,.0));

    // 4 triangles
    Tessalation* t = new Tessalation(Vector(0,0,0),100,0,mat);
    assert(t->getEdges()->size() == 6);
    assert(t->getVertices()->size() == 4);

    // 12 triangles
    t = new Tessalation(Vector(0,0,0),100,1,mat);
    assert(t->getVertices()->size() == 8);
    cout << t->getEdges()->size() << endl;
  //  assert(t->getEdges()->size() == 4 * 3);

    // 36 triangles
    t = new Tessalation(Vector(0,0,0),100,2,mat);
    assert(t->getVertices()->size() == 20);

    // 108 triangles
    t = new Tessalation(Vector(0,0,0),100,3,mat);
    assert(t->getVertices()->size() == 56);
}

void extrusion_test() {

    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    // Check bounds 
    Vector o = Vector(0,0,0);
    Vector top = Vector(10,0,0);
    BoundingBox b = BoundingBox(Vector(-1,-10,-10),Vector(11,10,10));
	    
    Extrusion* c = new Extrusion(o,top,9.0,5,m);
    assert(b.inside(c->boundingBoundingBox()));

    c = new Extrusion(top,o,9.0,5,m);
    assert(b.inside(c->boundingBoundingBox()));

    top = Vector(0,10,0);
    b = BoundingBox(Vector(-10,-1,-10),Vector(10,11,10));
    c = new Extrusion(o,top,5.0,5,m);
    assert(b.inside(c->boundingBoundingBox()));

    // Check intersection 
    c = new Extrusion(Vector(0,0,0),Vector(0,0,-10),5.0,3,m);
    c->prepare();
    KdTree* bsp = new KdTree();
    c->addSelf(bsp);
    bsp->prepare();
    Intersection i;
    Ray r = Ray(Vector(0.5,0.5,100),Vector(0,0,-1),1);
    assert(bsp->intersect(r,&i));

    // Check generated mesh 
    c = new Extrusion(Vector(0,0,0),Vector(0,0,-10),2.0,5,m);
    c->prepare();
    assert(c->getVertices()->size() == 5*2 + 2);

    Circle circle1 = Circle(Vector(0,75,0),200,Vector(0,1,0));
    Extrusion torus = Extrusion(circle1,100,16,10,new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.20,30));
    torus.prepare();

    assert(torus.getVertices()->size() == 16*10);

}

void cylinder_test() {
    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Cylinder* cyl = new Cylinder(Vector(0,0,0),Vector(0,0,10),10,m);

    /* Test inside() and onEdge() */
    assert(cyl->inside(Vector(0,0,5)));
    assert(cyl->inside(Vector(0,0,9)));
    assert(!cyl->inside(Vector(0,0,10)));
    
    assert(cyl->onEdge(Vector(0,0,10)));
    assert(cyl->onEdge(Vector(10,0,5)));
    assert(cyl->onEdge(Vector(-10,0,5)));
    assert(!cyl->inside(Vector(10,0,5)));
    assert(!cyl->inside(Vector(0,0,11)));
    assert(!cyl->inside(Vector(1,0,-1)));

    /* Test intersection() of z-axis aligned cylinder */
    
    assert(intersects(cyl,Vector(0,-1000,5),Vector(0,1,0)));
    assert(iPoint(cyl,Vector(0,-1000,5),Vector(0,1,0)) == Vector(0,-10,5));

    assert(intersects(cyl,Vector(0,1000,5),Vector(0,-1,0)));
    assert(iPoint(cyl,Vector(0,1000,5),Vector(0,-1,0)) == Vector(0,10,5));

    delete cyl;

    // Test a cylinder translated along the z-axis
    cyl = new Cylinder(Vector(0,0,2),Vector(0,0,10),10,m);

    assert(cyl->inside(Vector(0,0,3)));
    assert(cyl->inside(Vector(0,0,5)));
    assert(cyl->inside(Vector(0,0,9)));
    assert(!cyl->inside(Vector(0,0,10)));
    assert(!cyl->inside(Vector(0,0,11)));
    assert(!cyl->inside(Vector(0,0,2)));
    
    assert(cyl->onEdge(Vector(0,0,2)));
    assert(cyl->onEdge(Vector(0,0,10)));
    assert(cyl->onEdge(Vector(10,0,5)));
    assert(cyl->onEdge(Vector(-10,0,5)));
    assert(!cyl->inside(Vector(10,0,5)));
    assert(!cyl->inside(Vector(0,0,11)));
    assert(!cyl->inside(Vector(0,0,1)));
    assert(!cyl->onEdge(Vector(0,0,1)));
    delete cyl;
    
    // Test an x-axis aligned cylinder
    cyl = new Cylinder(Vector(2,0,0),Vector(10,0,0),10,m);
    assert(cyl->inside(Vector(5,0,0)));
    assert(cyl->inside(Vector(9,0,0)));
    assert(!cyl->inside(Vector(0,0,0)));
    assert(!cyl->inside(Vector(-1,0,0)));
    assert(cyl->onEdge(Vector(2,0,0)));
    assert(cyl->onEdge(Vector(10,0,0)));
    assert(cyl->onEdge(Vector(5,10,0)));
    assert(cyl->onEdge(Vector(5,-10,0)));
    assert(intersects(cyl,Vector(3,0,1000),Vector(0,0,-1)));
    delete cyl;
    
    // Test an y-axis aligned cylinder
    cyl = new Cylinder(Vector(0,2,0),Vector(0,10,0),10,m);
    assert(cyl->inside(Vector(0,5,0)));
    delete cyl;

    cyl = new Cylinder(Vector(1,1,1),Vector(10,10,10),10,m);
    assert(cyl->inside(Vector(2,2,2)));
    assert(cyl->inside(Vector(5,5,5)));
    assert(cyl->inside(Vector(7,7,7)));
    assert(cyl->inside(Vector(9,9,9)));
    assert(!cyl->onEdge(Vector(2,2,2)));
    assert(cyl->onEdge(Vector(1,1,1)));
    assert(cyl->onEdge(Vector(10,10,10)));

}

void test_3ds() {
    ThreeDS* chair = new ThreeDS("../3ds/egg-chair.3ds",1.0);
    assert(chair != NULL);
}

void objectgroup_test() {
    // Test clone()
    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Linesegment segment = Linesegment(Vector(0,0,0),Vector(0,0,100)); 
    Necklace* g1 = new Necklace(segment,10,10,m);
    SceneObject* g2 = g1->clone();
    g1->transform(Matrix::matrixTranslate(Vector(0,-100,0)));
    g2->transform(Matrix::matrixTranslate(Vector(0,100,0)));

    KdTree* bsp = new KdTree();
    g1->addSelf(bsp);
    g2->addSelf(bsp);
    bsp->prepare();

    Intersection i;
    Ray r = Ray(Vector(0,100,1000),Vector(0,0,-1),1);
    assert(bsp->intersect(r,&i));
    r = Ray(Vector(0,-100,1000),Vector(0,0,-1),1);
    assert(bsp->intersect(r,&i));
    r = Ray(Vector(0,0,1000),Vector(0,0,-1),1);
    assert(!bsp->intersect(r,&i));
}

void torus_test() {
    Material* m = new Material(RGB(1.0,0.2,0.2),0.75,RGB(1.0,1.0,1.0),0.75,30);
    Torus* t = new Torus(10,1,m);

    // inside
    assert(t->inside(Vector(10,0,0)));
    assert(t->inside(Vector(10.5,0,0)));
    assert(t->inside(Vector(9.5,0,0)));
    assert(!t->inside(Vector(11.5,0,0)));
    assert(!t->inside(Vector(11,0,0)));
    assert(!t->inside(Vector(9,0,0)));
    assert(t->inside(Vector(0,0,10)));
    assert(t->inside(Vector(0,-0.25,-10)));
    assert(t->inside(Vector(-10,0.25,0)));

    // onEdge
    assert(t->onEdge(Vector(11,0,0)));
    assert(t->onEdge(Vector(9,0,0)));
    assert(t->onEdge(Vector(0,0,-11)));
    assert(t->onEdge(Vector(0,0,-9)));
    assert(!t->onEdge(Vector(10,0,0)));
    assert(!t->onEdge(Vector(-10,0,0)));
    assert(!t->onEdge(Vector(0,0,-10)));
    assert(!t->onEdge(Vector(11,0.1,0)));

    // intersect
    Intersection i;
    //assert(i->getPoint() == Vector(0,0,11));
    //assert(t->normal(*i) == Vector(0,0,1));
    assert(intersects(t,Vector(0,0,1000),Vector(0,0,-1)));
    assert(iPoint(t,Vector(0,0,1000),Vector(0,0,-1)) == Vector(0,0,11));
    assert(iNormal(t,Vector(0,0,1000),Vector(0,0,-1)) == Vector(0,0,1));

    Ray ray = Ray(Vector(0,0,-1000),Vector(0,0,1),1);
    //assert(t->intersect(ray));
    assert(intersects(t,ray));
    assert(iPoint(t,ray) == Vector(0,0,-11));
    assert(iNormal(t,ray) == Vector(0,0,-1));

    ray = Ray(Vector(0,1000,0),Vector(0,-1,0),1); // In middle
    //assert(!t->intersect(ray));
    assert(!intersects(t,ray));
    ray = Ray(Vector(0,1000,11.1),Vector(0,-1,0),1); // Close by
    assert(!intersects(t,ray));
    //assert(!t->intersect(ray));
    ray = Ray(Vector(1000,11.1,0),Vector(-1,0,0),1); // Close by
    assert(!intersects(t,ray));
    //assert(!t->intersect(ray));
}

void transformed_instance_test() {
    Sphere* s = new Sphere(Vector(0,0,0),10.0,NULL);
    TransformedInstance* t1 = new TransformedInstance(s);
    t1->transform(Matrix::matrixTranslate(Vector(100,0,0)));
    TransformedInstance* t2 = new TransformedInstance(s);
    t2->transform(Matrix::matrixTranslate(Vector(-100,0,0)));
    assert(intersects(t1,Vector(100,0,1000),Vector(0,0,-1)));
    assert(!intersects(t1,Vector(0,0,1000),Vector(0,0,-1)));

    assert(intersects(t2,Vector(-100,0,1000),Vector(0,0,-1)));
    assert(!intersects(t2,Vector(0,0,1000),Vector(0,0,-1)));
    
    //assert(t1->intersect(Ray,0)));
    //assert(!t1->intersect(Ray(Vector(0,0,1000),Vector(0,0,-1),0)));
    //assert(t2->intersect(Ray(Vector(-100,0,1000),Vector(0,0,-1),0)));
    //assert(!t2->intersect(Ray(Vector(0,0,1000),Vector(0,0,-1),0)));
}

int main(int argc, char *argv[]) {
    transformed_instance_test();
    sphere_test();
    box_test();
    cylinder_test();
    torus_test();
    mesh_test();
    tetrahedron_test();
    tesselation_test();
    extrusion_test();
    objectgroup_test();

    Mesh::test();
    test_3ds();
    boolean_test();
    return EXIT_SUCCESS;
}


