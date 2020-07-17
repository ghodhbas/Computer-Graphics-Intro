#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "SceneParser.h"
#include "Ray.h"
#include "Hit.h"

float frand(float LO, float HI);

#define EPSILON 0.001
#define M_PI 3.14159265358979323846f

class SceneParser;

class RayTracer
{
public:

	RayTracer()
	{
		assert(false);
	}

	RayTracer(SceneParser* scene, int max_bounces, int shadows //more arguments as you need...
	);
	~RayTracer();

	Vector3f traceRay(Ray& ray, float tmin, int bounces,
		float refr_index, Hit& hit) const;
private:
	SceneParser* m_scene;
	int shadows;
	int m_maxBounces;
};

#endif // RAY_TRACER_H
