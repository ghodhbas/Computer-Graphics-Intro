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


Vector3f validPixel(Image& image, int i, int j) {

	if (i >= 0 && i < image.Width() && j >= 0 && j < image.Height()) {
		return image.GetPixel(i, j);
	}
	else {

		//instead of returning balck return the closest valid!
		return Vector3f::ZERO;
	}
}

void blur(Image& image , Image& blured) {
	float K[5] = { 0.1201, 0.2339, 0.2931, 0.2339, 0.1201 };
	int width = image.Width();
	int height = image.Height();
	Image blurH = Image(width, height);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Vector3f bluredH = K[0] * validPixel(image, i, j - 2) +
				K[1] * validPixel(image, i, j - 1) +
				K[2] * validPixel(image, i, j) +
				K[3] * validPixel(image, i, j + 1) +
				K[4] * validPixel(image, i, j + 2);
			blurH.SetPixel(i, j, bluredH);
		}
	}

	//NEED TO DO SOMETHING ABOUT THIS VAR
	Image* blurV = new Image(width, height);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Vector3f bluredV = K[0] * validPixel(blurH, i - 2, j) +
				K[1] * validPixel(blurH, i - 1, j) +
				K[2] * validPixel(blurH, i, j) +
				K[3] * validPixel(blurH, i + 1, j) +
				K[4] * validPixel(blurH, i + 2, j);
			blurV->SetPixel(i, j, bluredV);
		}
	}
	
	blured = *blurV;
}

void
Renderer::render(int width,
	int height,
	int max_bounces,
	const std::string& outputFilename,
	int shadows, int jitter)
{

	if (jitter) {
		width *= 3;
		height *= 3;
	}

	//-sp contains all scene data
	Image image = Image(width, height);
	//Image depth_image = Image(width, height);
	//Image normal_image = Image(width, height);
	Camera* camera = _sp.getCamera();
	//raytracer
	RayTracer raytracer(&_sp, max_bounces, shadows);
	
	
	float x_step = 2.f / width;
	float y_step = 2.f / height;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			
			Vector3f color = Vector3f::ZERO;
			Vector3f depth = Vector3f::ZERO;
			Ray ray = camera->generateRay(Vector2f(-1.f + x_step * +i, -1.f + y_step * j));;
			//supersample
			if (jitter) {
				ray = camera->generateRay(Vector2f(-1.f + x_step * (frand(-0.5f, 0.5f) + i), -1.f + y_step * (frand(-0.5f, 0.5f) + j)));
			}
			
			Hit hit = Hit();
			color += raytracer.traceRay(ray, camera->getTMin(), 0, 1.f, hit);
			
			if (i % 50 == 0 && j % width== 50) {
				std::cout << "i=" << i << "\tj=" << j << std::endl;
			}

			//REGULAR RENDER
			image.SetPixel(i, j, color);
		}
	}


	//downsample
	if (jitter) {
		std::cout << "Downsampling..."<< std::endl;
		Image blured = Image(image.Height(), image.Width());
		blur(image, blured);
		Image output = Image(image.Height() / 3, image.Width() / 3);
		for (int oi = 0; oi < width/3; oi++) {
			for (int oj = 0; oj < height/3; oj++) {
				int i = 3 * oi + 1;
				int j = 3 * oj + 1;
				Vector3f downSample = validPixel(blured, i - 1, j - 1) + validPixel(blured, i - 1, j) + validPixel(blured, i - 1, j + 1) +
					validPixel(blured, i, j - 1) + validPixel(blured, i, j) + validPixel(blured, i, j + 1) +
					validPixel(blured, i + 1, j - 1) + validPixel(blured, i + 1, j) + validPixel(blured, i + 1, j + 1);
				output.SetPixel(oi, oj, downSample / 9.0);
			}
		}
		output.SaveBMP(outputFilename.c_str());
	}else
	image.SaveBMP(outputFilename.c_str());
	
	
	
}