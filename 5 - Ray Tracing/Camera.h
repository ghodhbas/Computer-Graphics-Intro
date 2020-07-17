#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay(const Vector2f& point) = 0;

	virtual float getTMin() const = 0;
	virtual ~Camera() {}
protected:
	Vector3f center;
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;
};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction, const Vector3f& up, float angle) {
		this->center = center;
		this->direction = direction.normalized();
		this->up = up.normalized();
		this->horizontal = Vector3f::cross(this->direction, up).normalized();
		D = 1.f / std::tanf(angle / 2.f);
	}

	// point (x,y) is in range (-1,-1) and (1,1)
	virtual Ray generateRay(const Vector2f& point) {
		//Vector3f direction; -- aspect ratio can be multipliedto y component
		Vector3f r = point.x() * horizontal + point.y() * up + D * direction;
		r.normalize();
		return Ray(center, r);
	}

	virtual float getTMin() const {
		return 0.0f;
	}

private:
	float D;
};

#endif //CAMERA_H
