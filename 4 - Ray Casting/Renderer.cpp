#include "Renderer.h"
#include <signal.h>
#include "Image.h"
#include "Camera.h"
#include "Light.h"
#include "Ray.h"
#include "Hit.h"

Renderer::Renderer(const char* filename) :
	_sp(filename)
{
}

Renderer::~Renderer()
{}

void
Renderer::Render(int width,
    int height,
    const std::string& outputFilename,
    float minDepth,
    float maxDepth,
    const std::string& depthFilename,
    const std::string& normalFilename)
{

    //-sp contains all scene data
    Image image = Image(width, height);
    Image depth_image = Image(width, height);
    Image normal_image = Image(width, height);
    Camera* camera = _sp.getCamera();
    Group* group = _sp.getGroup();
    Vector3f ambientLight = _sp.getAmbientLight();


    float x_step = 2.f / width;
    float y_step = 2.f / height;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {

            Ray ray = camera->generateRay(Vector2f(-1.f + x_step * i, -1.f + y_step * j));
            Hit hit = Hit();
            //color
            Vector3f color = ambientLight;
            Vector3f depth = Vector3f::ZERO;
            Vector3f n = Vector3f::ZERO;
            //intersect ray with main group
            if (group->intersect(ray, hit, camera->getTMin())) {
                float t = hit.getT();
                //get location of hit
                Vector3f p = ray.pointAtParameter(t);
                //material
                Material* mat = hit.getMaterial();
                //ambient light
                color = color * mat->getDiffuseColor();

                //light
                for (size_t i = 0; i < _sp.getNumLights(); i++)
                {
                    Light* l = _sp.getLight(i);
                    Vector3f dir_toLight;
                    Vector3f light_color;
                    float distance_toLight;
                    l->getIllumination(p, dir_toLight, light_color, distance_toLight);
                    //shade - diffuse + specular
                    color += mat->Shade(ray, hit, dir_toLight, light_color);
                }


                //REGULAR RENDER
                image.SetPixel(i, j, color);


                //DEPTH IMAGE RENDER
                if (t >= minDepth && t <= maxDepth) {
                    float level = (maxDepth - t) / (maxDepth - minDepth);
                    depth_image.SetPixel(i, j, level * Vector3f(1, 1, 1));
                }
                else {
                    depth_image.SetPixel(i, j, Vector3f(1, 1, 1));
                }

                //Normal Image Render
                Vector3f n_color = Vector3f(std::abs(hit.getNormal().x()), std::abs(hit.getNormal().y()), std::abs(hit.getNormal().z()));
                normal_image.SetPixel(i, j, n_color);
            }
            else {
                //render background when there is no intersection
                image.SetPixel(i, j, _sp.getBackgroundColor());
            }
            
            
        }
    }


    //SAVE RENDERS
    image.SaveBMP(outputFilename.c_str());
    if (&depthFilename != NULL) {
        depth_image.SaveBMP(depthFilename.c_str());
    }
    if (&normalFilename != NULL) {
        normal_image.SaveBMP(normalFilename.c_str());
    }
}