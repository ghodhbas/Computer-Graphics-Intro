#include "Renderer.h"
#include <signal.h>
#include "Image.h"
#include "Camera.h"
#include "Light.h"
#include "Ray.h"
#include "Hit.h"

Renderer::Renderer(ArgParser& parser) :
	_sp(parser.input_file)
{
	argparser = parser;
}

Renderer::~Renderer()
{}


Vector3f validPixel(Image& image, int i, int j) {

	if (i >= 0 && i < image.Width() && j >= 0 && j < image.Height()) {
		return image.GetPixel(i, j);
	}
	else {
		// Addition: instead of returning balck return the closest valid!
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

void downsample(Image& image, int width , int height, Image& output) {
	for (int oi = 0; oi < width / 3; oi++) {
		for (int oj = 0; oj < height / 3; oj++) {
			int i = 3 * oi + 1;
			int j = 3 * oj + 1;
			Vector3f downSample = validPixel(image, i - 1, j - 1) + validPixel(image, i - 1, j) + validPixel(image, i - 1, j + 1) +
				validPixel(image, i, j - 1) + validPixel(image, i, j) + validPixel(image, i, j + 1) +
				validPixel(image, i + 1, j - 1) + validPixel(image, i + 1, j) + validPixel(image, i + 1, j + 1);
			output.SetPixel(oi, oj, downSample / 9.0);
		}
	}
}

void
Renderer::render()
{	
	//retrive needed variable from argument line
	int width = argparser.width;
	int height = argparser.height;
	int max_bounces = argparser.bounces;
	int shadows = argparser.shadows;
	int jitter = argparser.jitter;
	int filter = argparser.filter;
	string outputFilename = argparser.output_file;
	string depthFilename = argparser.depth_file;
	float minDepth = argparser.depth_min;
	float maxDepth = argparser.depth_max;
	string normalFilename = argparser.normals_file;


	//outputs
	Image depth_image = Image(width, height);
	Image normal_image = Image(width, height);
	if (jitter) {
		width *= 3;
		height *= 3;
	}
	Image image = Image(width, height);
	//camera
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
			//run ray tracer
			Hit hit = Hit();
			color += raytracer.traceRay(ray, camera->getTMin(), 0, 1.f, hit);
			
			//printing for debugging
			//if (i % 50 == 0 && j % width== 50) {
			//	std::cout << "i=" << i << "\tj=" << j << std::endl;
			//}

			//REGULAR RENDER
			image.SetPixel(i, j, color);
			//Depth Image
			//DEPTH IMAGE RENDER
			if (depthFilename != "") {
				//adjusting for super sampling
				int row = i;
				int col = j;
				if (jitter) {
					row = i / 3;
					col = j / 3;
				}			
				if (hit.getT() >= minDepth && hit.getT() <= maxDepth) {
					float level = (maxDepth - hit.getT()) / (maxDepth - minDepth);
					depth_image.SetPixel(row, col, level * Vector3f(1, 1, 1));
				}
				else {
					depth_image.SetPixel(row, col, Vector3f(1, 1, 1));
				}
			}
			//Normal image
			if (normalFilename != "") {
				//adjusting for super sampling
				int row = i;
				int col = j;
				if (jitter) {
					row = i / 3;
					col = j / 3;
				}
				Vector3f n_color = Vector3f(std::abs(hit.getNormal().x()), std::abs(hit.getNormal().y()), std::abs(hit.getNormal().z()));
				normal_image.SetPixel(row, col, n_color);
			}

		}
	}

	//add filter option
	//downsample
	if (jitter) {
		std::cout << "Downsampling..."<< std::endl;
		Image blured = Image(width, height);
		Image output = Image(width/ 3, height/ 3);
		//apply gaussian blur
		if (filter) {
			blur(image, blured);
			downsample(blured, width, height, output);
		}else
			downsample(image, width, height, output);

		output.SaveBMP(outputFilename.c_str());
	}else
	image.SaveBMP(outputFilename.c_str());
	

	if (depthFilename != "") {
		depth_image.SaveBMP(depthFilename.c_str());
	}
	if (normalFilename != "") {
		normal_image.SaveBMP(normalFilename.c_str());
	}
	
	
}