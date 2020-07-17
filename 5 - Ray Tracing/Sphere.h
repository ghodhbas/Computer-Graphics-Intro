#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <algorithm>    // std::min

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere : public Object3D
{
public:
	Sphere() {
		//unit ball at the center
		center = Vector3f(0.f, 0.f, 0.f);
		radius = 1.f;
	}

	Sphere(Vector3f center, float radius, Material* material) :Object3D(material) {
		this->center = center;
		this->radius = radius;
		this->material = material;
	}

	~Sphere() {}

	virtual bool intersect(const Ray& r, Hit& h, float tmin) {
		float t;
		Vector3f origin = r.getOrigin();
		Vector3f dir = r.getDirection();

		float a = Vector3f::dot(dir, dir);
		//float b = 2.f * Vector3f::dot(r.getDirection(), r.getOrigin());
		//float c = Vector3f::dot(r.getOrigin(), r.getOrigin()) - (radius * radius);
		float b = 2 * (Vector3f::dot(origin, dir) - Vector3f::dot(dir, center));
		float c = origin.absSquared() - 2 * Vector3f::dot(origin, center) + center.absSquared() - radius * radius;

		float delta = b * b - 4.f * a * c;
		//3 case	- no Intersection
		if (delta < 0.f)
		{
			return false;
		}
		//			- 1  Intersetion
		else if (delta == 0.f) {
			t = (-1.f * b) / (2.f * a);
		}
		//			- 2 intersections - choose smallest positive
		else
		{
			float t1 = (-b + sqrt(delta)) / (2.f * a);
			float t2 = (-b - sqrt(delta)) / (2.f * a);
			// t1 always bigger than t2
			if (t2 < 0.f) t = t1;
			else t = t2;
		}
		//new closer intersection point
		if (t > tmin && t < h.getT()) {
			//calculate new normal
			Vector3f n = (r.pointAtParameter(t) - center).normalized();
			h.set(t, this->material, n);
			return true;
		}

		return false;
	}

protected:
	float radius;
	Vector3f center;
};

#endif
