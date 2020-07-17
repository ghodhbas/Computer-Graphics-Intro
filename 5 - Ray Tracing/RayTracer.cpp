#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection(const Vector3f& normal, const Vector3f& incoming)
{
	Vector3f R = incoming - 2.f * (Vector3f::dot(incoming, normal)) * normal;
	return R.normalized();
}

bool transmittedDirection(const Vector3f& normal, const Vector3f& incoming,
	float index_n, float index_nt,
	Vector3f& transmitted)
{
	float cos = Vector3f::dot(incoming, normal);
	float ratio = index_n / index_nt;
	float check = 1 - std::pow(ratio, 2) * (1 - std::pow(cos, 2));
	if (check > 0) {
		transmitted = ratio * (incoming - cos * normal) - std::sqrt(check) * normal;
		transmitted.normalize();
		return true;
	}
	return false;
}

float frand(float LO, float HI) {
	return  LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

/*create local coordiinate around the normal -- used to draw a random reflection ray*/
void createCoordinateSystem(const Vector3f N, Vector3f& Nt, Vector3f& Nb)
{
	if (fabs(N.x()) > fabs(N.y()))
		Nt = Vector3f(N.z(), 0.f, -N.x()) / sqrtf(N.x() * N.x() + N.z() * N.z());
	else
		Nt = Vector3f(0.f, -N.z(), N.y()) / sqrtf(N.y() * N.y() + N.z() * N.z());
	Nb = Vector3f::cross(N, Nt);
}

RayTracer::RayTracer(SceneParser* scene, int max_bounces, int shadows
	//more arguments if you need...
) :
	m_scene(scene)
	
{
	
	m_maxBounces = max_bounces;
	this->shadows = shadows;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay(Ray& ray, float tmin, int bounces,
	float refr_index, Hit& hit) const
{
	Vector3f color = Vector3f::ZERO;
	//test depth
	if (bounces > m_maxBounces) return color;

	Group* group = m_scene->getGroup();
	Camera* camera = m_scene->getCamera();
	Vector3f ambientLight = m_scene->getAmbientLight();

	//intersect ray with main group
	if (group->intersect(ray, hit, tmin)) {
		float t = hit.getT();
		//get location of hit
		Vector3f p = ray.pointAtParameter(t);
		//material
		Material* mat = hit.getMaterial();

		//calculate light influence
		for (int i = 0; i < m_scene->getNumLights(); i++)
		{
			Light* l = m_scene->getLight(i);
			Vector3f dir_toLight;
			Vector3f light_color;
			float distance_toLight;
			l->getIllumination(p, dir_toLight, light_color, distance_toLight);
			//shadows
			if (shadows) {
				Ray shadow_ray = Ray(p, dir_toLight);
				Hit shadow_hit = Hit();
				group->intersect(shadow_ray, shadow_hit, EPSILON);
				if (shadow_hit.getT() >= distance_toLight) {
					color += mat->Shade(ray, hit, dir_toLight, light_color);
				}
			}
			else {
				//shade - diffuse 
				color += mat->Shade(ray, hit, dir_toLight, light_color);
			}
			
		}

		// add ambient light --- might need to be fixed (change diffuse color to K_a)
		if (hit.hasTex && mat->t.valid()) {
			// add ambient to texture
			color += ambientLight * mat->t(hit.texCoord[0], hit.texCoord[1]);
		}
		else {
			color += ambientLight * mat->getDiffuseColor();
		}

		//mirror reflection
		if (bounces < m_maxBounces) {
			Vector3f n = hit.getNormal().normalized();
			//reflection
			Vector3f mirrorDir = mirrorDirection(n, ray.getDirection().normalized());
			Ray mirrorRay = Ray(p, mirrorDir);
			Hit mir_h = Hit();
			Vector3f reflColor = mat->getSpecularColor() *  traceRay(mirrorRay, EPSILON, refr_index, bounces + 1, mir_h);

			//total reflection
			float newIndex = mat->getRefractionIndex();
			if (newIndex <= 0.f) {
				color += reflColor;
			}
			else {
				//going into the solid not outwards
				if (newIndex == refr_index) {
					newIndex = 1.0f;
					hit.set(hit.getT(), mat, -hit.getNormal());
				}
				//refraction
				Vector3f transmitted;
				Vector3f refrColor;
				if (transmittedDirection(n, ray.getDirection(), refr_index, mat->getRefractionIndex(), transmitted)) {
					Ray RefRay = Ray(p, transmitted);
					Hit refr_h = Hit();
					refrColor = mat->getSpecularColor() * traceRay(RefRay, EPSILON, bounces + 1, newIndex, refr_h);
				}

				//blend refraction and reflection
				float R0 = std::pow((newIndex - refr_index) / (newIndex + refr_index), 2);
				float c = refr_index <= newIndex ? std::abs(Vector3f::dot(ray.getDirection(), n)) : std::abs(Vector3f::dot(transmitted, n));

				float R = R0 + (1 - R0) * std::pow(1 - c, 5);
				color += R * reflColor + (1.f - R) * refrColor;
			}
		}
		return color;
	}
	//no intersection
	else {
		//render background when there is no intersection
		return m_scene->getBackgroundColor(ray.getDirection());
	}
}