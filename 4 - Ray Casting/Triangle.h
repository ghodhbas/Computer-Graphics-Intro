#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
		this->a = a;
		this->b = b;
		this->c = c;
		
        hasTex = false;
	}

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin){
		Vector3f r_d = ray.getDirection();
		Vector3f r_o = ray.getOrigin();

		Matrix3f A = Matrix3f(a.x() - b.x(), a.x() - c.x(), r_d.x(),
			a.y() - b.y(), a.y() - c.y(), r_d.y(),
			a.z() - b.z(), a.z() - c.z(), r_d.z());
		float detA = A.determinant();

		Matrix3f t_cramer = Matrix3f(a.x() - b.x(), a.x() - c.x(), a.x() - r_o.x(),
			a.y() - b.y(), a.y() - c.y(), a.y() - r_o.y(),
			a.z() - b.z(), a.z() - c.z(), a.z() - r_o.z());

		Matrix3f beta_cramer = Matrix3f(a.x() - r_o.x(), a.x() - c.x(), r_d.x(),
			a.y() - r_o.y(), a.y() - c.y(), r_d.y(),
			a.z() - r_o.z(), a.z() - c.z(), r_d.z());

		Matrix3f gamma_cramer = Matrix3f(a.x() - b.x(), a.x() - r_o.x(), r_d.x(),
			a.y() - b.y(), a.y() - r_o.y(), r_d.y(),
			a.z() - b.z(), a.z() - r_o.z(), r_d.z());

		float alpha, beta, gamma, t;
		t = t_cramer.determinant() / detA;
		beta = beta_cramer.determinant() / detA;
		gamma = gamma_cramer.determinant() / detA;
		alpha = 1 - beta - gamma;
		if (t >= tmin && t < hit.getT() && beta + gamma <= 1 && beta >= 0 && gamma >= 0) {
			Vector3f normal = alpha * normals[0] + beta * normals[1] + gamma * normals[2];
			hit.set(t, this->material, normal.normalized());
			//texture positioning
			hit.setTexCoord(alpha * texCoords[0] + beta * texCoords[1] + gamma * texCoords[2]);
			return true;
		}
		return false;
	}
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
	Vector3f a, b, c;

};

#endif //TRIANGLE_H
