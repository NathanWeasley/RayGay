#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <iosfwd>
#include "image/rgb.h"

class Intersection;
class Ray;
class Lightsource;
class SceneObject;
class Matrix;
class Sphere;
class Image;
class ObjectCollection;
class Camera;
class SpaceSubdivider;

/// The collection of objects and lights.

class Scene {

    public:
        // Constructor
	Scene();

	/// Destructor
	virtual ~Scene();	

	/// Place an object in the scene
	void addObject(SceneObject* obj);
	
	/// Place a lightsource in the scene
	void addLight(Lightsource* light);

	/// Place the camera in the scene
	void setCamera(Camera* camera);

	/// Returns the camera placed in the scene
	Camera* getCamera() const;

	/// Transforms all objects in the scene
	void transform(const Matrix& m);

	/// Returns a vector of all lightsources
	std::vector<Lightsource*> getLightsources();
	
	/// Returns a vector of all objects
	std::vector<SceneObject*> getObjects();

	/// Set the background color
	void setBackgroundColor(const RGB& c) { bg_color = c; };

	/// Returns the background color
	RGB getBackgroundColor(const Ray& ray) const;

	/// Sets the environmentmap
	void setEnvironmentMap(const std::string& filename);

	void setFog(const RGB& color, const double distance);
	bool fogEnabled() const { return fog_enabled; };
	RGB getFogColor() const { return fog_color; };
	double getFogDistance() const { return fog_distance; };

	/// Add all objects in scene to spacesubdivider
	void initSpace(SpaceSubdivider* space);

    private:
	std::vector<Lightsource*> lights;
	std::vector<SceneObject*> objects;
	Image* environmentMap;
	Sphere* environmentSphere;
	Camera* camera;
	RGB bg_color;

	bool fog_enabled;
	RGB fog_color;
	double fog_distance;
};

#endif

