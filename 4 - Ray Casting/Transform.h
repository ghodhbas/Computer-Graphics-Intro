#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
 Transform( const Matrix4f& m, Object3D* obj ):o(obj){
     this->m = m;
  }
  ~Transform(){
  }
  virtual bool intersect( const Ray& r , Hit& h , float tmin){
      Matrix4f Minv = this->m.inverse();
      // transform the ray -- normalize??
      Ray newRay = Ray((Minv * Vector4f(r.getOrigin(), 1.0f)).xyz(), (Minv * Vector4f(r.getDirection(), 0.0f)).xyz());
      if (o->intersect(newRay, h, tmin)) {
          h.set(h.getT(), h.getMaterial(), (Minv.transposed() * Vector4f(h.getNormal(), 0.0f)).xyz().normalized());
          return true;
      }
      return false;
  }

 protected:
  Object3D* o; //un-transformed object	
  Matrix4f m;
};

#endif //TRANSFORM_H
