
#ifndef SPACE_KD_TREE_H
#define SPACE_KD_TREE_H

#include "math/vector.h"
#include "spacesubdivider.h"
#include "boundingbox.h"
#include "objects/object.h"
#include <vector>

/*
class Ray;
class Intersection;
class Object;
*/
#define KD_TREE_MAX 3
#define KD_TREE_MAX_DEPTH 100

/**
 * k-dimensional tree.
 *
 * Implementerer http://www.acm.org/jgt/papers/HavranKopalBittnerZara97/ som er den bedste BSP-traversal algorithme.
 */
class KdTree : public SpaceSubdivider {

    public:
	/// Constructor
	KdTree();
	/// Desctructor
	virtual ~KdTree();
	bool intersect(const Ray& ray, Intersection* result) const; 
	//bool intersect(const Ray& ray) const; ///< Returns the nearest intersection

	bool intersectPrimary(const Ray& ray, Intersection* result) const; ///< Returns the nearest intersection
	Object* intersectForShadow(const Ray& ray, double max_t) const; ///< Returns any intersection 

	void addObject(Object* obj); ///< Place a object in the kd-tree 
	void prepare();

	/// The BoundingBox around all objects added to the tree
	BoundingBox boundingBox() const { return world_bbox; };

    private:
	class KdNode {
	    public:
		KdNode* left;     // Left child
		union {
		    std::vector<Object*>* objects;  // Enclosed objects when this is a leaf
		    KdNode* right;    // Right child
		};
		float splitPlane; // Position of splitting plane
		int axis;         // Orientation where x,y,z is 0,1,2 and -1 denotes a leaf
	    public:
		KdNode();
	};
	class KdNodeTmp {
	    public:
		std::vector<Object*>* objects;  // Enclosed objects when this is a leaf
		int left;     // Left child
		int right;    // Right child
		float splitPlane; // Position of splitting plane
		int axis;         // Orientation where x,y,z is 0,1,2 and -1 denotes a leaf
	    public:
		KdNodeTmp();
	};

	struct StackElem {
	    KdNode* node;   // pointer to far child
	    float t;        // the entry/exit signed distance
	    Vector pb;      // coordinates of entry/exit point
	    int prev;       // pointer to previus stack item
	};

	bool intersect(const Ray& ray, Intersection* result, const double a, const double b) const;
	Object* intersectForShadow(const Ray&,const double, const double) const;
	int largestDimension(const BoundingBox& box);
	BoundingBox enclosure(std::vector<Object*>* objects) const;
	BoundingBox world_bbox;
	double objectMedian(std::vector<Object*>* objects, int d) const;
	double spacialMedian(std::vector<Object*>* objects, int d) const;
	Vector measureSplit(std::vector<Object*>* objects, int dim, double val) const;

	void prepare(int curNode_idx, int depth);

	KdNode* nodes;
	std::vector<KdNodeTmp> tmp_nodes;
	int max_depth;

	std::vector<Object*>* added_objects;
	bool prepared;

};

#endif

