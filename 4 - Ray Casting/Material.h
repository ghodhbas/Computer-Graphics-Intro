#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
#include <algorithm>

///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
 Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0):
  diffuseColor( d_color),specularColor(s_color), shininess(s)
  {
        	
  }

  virtual ~Material()
    {

    }

  virtual Vector3f getDiffuseColor() const 
  { 
    return  diffuseColor;
  }
    

  Vector3f Shade( const Ray& ray, const Hit& hit,
                  const Vector3f& dirToLight, const Vector3f& lightColor ) {
     
      Vector3f textureColor = diffuseColor;
      //check for texture
      if (t.valid()) {
          textureColor = t(hit.texCoord[0], hit.texCoord[1]);
      }
      //reflection dir
      Vector3f R =  ray.getDirection() - 2.f * (Vector3f::dot(ray.getDirection(), hit.getNormal())) * hit.getNormal();
      Vector3f spec = specularColor * std::pow(std::max(0.f, Vector3f::dot(dirToLight, R)), shininess);
      Vector3f diff = textureColor * std::max(0.f, Vector3f::dot(hit.getNormal(), dirToLight));
      
     
      return  (diff + spec) * lightColor;
		
  }

  void loadTexture(const char * filename){
    t.load(filename);
  }

  Texture getTexture() { return t; }

 protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;
  Texture t;
};



#endif // MATERIAL_H
