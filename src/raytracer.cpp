#include "raytracer.h"

#include <iostream>
#include <math.h>

#include "math/vector.h"
#include "ray.h"
#include "intersection.h"
#include "object.h"
#include "image/rgb.h"
#include "math/matrix.h"
#include "scene.h"
#include "camera.h"
#include "image/image.h"
#include "lights/lightsource.h"

Raytracer::Raytracer() : Renderer() {
}

RGB Raytracer::getPixel(const Vector2& v) {
    Ray ray = scene->getCamera()->getRay(v[0],v[1]);
    return tracePrimary(ray);
}

RGB Raytracer::tracePrimary(const Ray& ray) {
    Stats::getUniqueInstance()->inc("Primary camera rays cast");
    bool intersected = space->intersectPrimary(ray);
    return traceSub(intersected, ray, 1);
}

RGB Raytracer::trace(const Ray& ray, int depth) {
    bool intersected = space->intersect(ray);
    return traceSub(intersected, ray, depth);
}

RGB Raytracer::traceSub(bool intersected, const Ray& ray, int depth) {
    Stats::getUniqueInstance()->inc("Total camera rays cast");
    RGB color; 
    Intersection intersection;
    double intersect_distance;

    if (intersected) {
	intersection = *(space->getLastIntersection());
	intersect_distance = (intersection.getPoint() - ray.getOrigin()).length();
	color = shade(ray,intersection,depth);
    } else {
        color = scene->getBackgroundColor(ray);
	intersect_distance = HUGE_DOUBLE;
    }

    if (scene->fogEnabled()) {
	double D = scene->getFogDistance();
	double v = expf(-intersect_distance/D);
	color = v * color + (1-v) * scene->getFogColor();
    }
    return color;
}

RGB Raytracer::shade(const Ray& ray, const Intersection& intersection, int depth) {
    Object* object = intersection.getObject();
    const Vector point = intersection.getPoint();
    Vector normal = object->normal(intersection);
    const Material& material = object->getMaterial();
    normal = material.bump(intersection,normal);
    double ambient_intensity = 0.2;
    RGB result_color = ambient_intensity * material.getDiffuseColor(intersection);
    vector<Lightsource*> lights = scene->getLightsources();
    for (vector<Lightsource*>::iterator p = lights.begin(); p != lights.end(); p++) {
	double attenuation = (*p)->getAttenuation(point);

	if (attenuation > double(0)) {
	    Lightinfo info = (*p)->getLightinfo(intersection,normal,space);
	    if (info.cos > 0.0) {
		RGB color = RGB(0.0,0.0,0.0);
		// Check for blocking objects
		if (info.intensity > 0.0) {
		    double intensity = info.intensity * attenuation;
		    // Diffuse color
		    color =  intensity * info.cos * material.getKd() * material.getDiffuseColor(intersection);

		    // Specular color (Phong)
		    Vector light_reflect = info.direction_to_light.reflect(normal);
		    light_reflect.normalize();
		    double rv = light_reflect * (-1 * ray.getDirection());
		    if (rv > 0.0) {
			rv = pow(rv,material.getSc());
			color = color + ( intensity * rv *  material.getKs() * material.getSpecularColor());
		    }
		}
		result_color = result_color + color;
	    } 
	}
    }
    if (depth < 4) {
	/* Bounce a reflection off the intersected object */
	if (material.getKs() > 0) {
	    Vector refl_vector = -1 * ray.getDirection();
	    refl_vector = refl_vector.reflect(normal);
	    refl_vector.normalize();
	    RGB refl_col = RGB(0.0,0.0,0.0);
	    if (material.glossEnabled()) {
		/* Distributed reflection */
		const double v = sin(material.glossMaxAngle());
		const double h = cos(material.glossMaxAngle());

		Vector corners[3];
		corners[0] = Vector(-v,v,h);
		corners[1] = Vector(v,v,h);
		corners[2] = Vector(-v,-v,h);
		corners[3] = Vector(v,-v,h);
		Matrix orient = Matrix::matrixOrient(refl_vector);
		orient = orient.inverse();
		for(unsigned int i = 0; i < 4; i++) {
		    corners[i] = orient*corners[i];
		    corners[i].normalize();
		}
		const unsigned int max_rays = material.glossRaysNum();
		Vector v1,v2;
		double x_factor,y_factor;
		for(unsigned int xx = 0; xx < max_rays; xx++) {
		    for(unsigned int yy = 0; yy < max_rays; yy++) {
			x_factor = (double(xx)+RANDOM(0,1)) / double(max_rays);
			y_factor = (double(yy)+RANDOM(0,1)) / double(max_rays);
			v1 = x_factor*corners[0] + (1-x_factor)*corners[1];
			v2 = x_factor*corners[2] + (1-x_factor)*corners[3];
			refl_vector =  y_factor*v1 + (1-y_factor)*v2;
			refl_vector.normalize();
			Ray refl_ray = Ray(point,refl_vector,ray.getIndiceOfRefraction());
			refl_col += trace(refl_ray, depth + 1);
		    }
		}
		refl_col *= 1.0/double(max_rays*max_rays);
	    } else {
		/* Single reflected ray */
		Ray refl_ray = Ray(point,refl_vector,ray.getIndiceOfRefraction());
		refl_col = trace(refl_ray, depth + 1);
	    }
	    result_color = result_color + material.getKs() * refl_col;
	}

	/* Should we send a ray through the intersected object? */
	if (material.transmission_coefficient > 0.0) {
	    // Calculate refraction vector (page 757)
	    double my = ray.getIndiceOfRefraction() / material.indice_of_refraction;
	    Vector I = -1 * ray.getDirection();
	    double n = normal * I;
	    double p = my*my*(1 - n*n);
	    if (p < 1) {
		// No internal reflection (page 758)
		Vector T = (my*n - sqrt(1 - p))*normal - my*I;
		Ray trans_ray = Ray(point,T,material.indice_of_refraction);
		RGB trans_col = trace(trans_ray, depth + 1);
		result_color = result_color + material.transmission_coefficient * trans_col;
	    }
	}
    }

    result_color.clip();
    return result_color;
}

