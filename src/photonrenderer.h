#ifndef PHOTON_RENDERER_H
#define PHOTON_RENDERER_H

#include "renderer.h"

class RGB;
class Ray;
class Object;
class Intersection;
class PhotonSettings;
class GlobalPhotonMap;
class CausticsMap;

/**
 * Implementation of Renderer that supply a raytracer using photonmaps.
 */
class PhotonRenderer : public Renderer {

    public:
	/// Default constructor
	PhotonRenderer(PhotonSettings* photonsettings);

	/// This populates the photon maps
	void init(Scene* scene, SpaceSubdivider* space);

    private:
	/// The photonmap to use
	GlobalPhotonMap* globalphotonmap;
	CausticsMap* causticsphotonmap;
	PhotonSettings* photonsettings;

	RGB getPixel(const Vector2& v);

	RGB shade(const Ray&, const Intersection&, int depth);
	RGB trace(const Ray&, int depth);
	RGB traceSub(bool intersected, const Ray&, int depth);
	RGB tracePrimary(const Ray&);
	Vector gatherIrradiance(const Vector& point, const Vector& normal,const Vector& raydir) const;
};


#endif
