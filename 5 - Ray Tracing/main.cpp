#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <ctime>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include "Renderer.h"
#include "ArgParser.h"

using namespace std;

float clampedDepth(float depthInput, float depthMin, float depthMax);

#include "bitmap_image.hpp"
int main(int argc, char* argv[])
{
	// Report help usage if no args specified.
	if (argc == 1) {
		std::cout << "Usage: a4 "
			<< "-input <scene> -size <width> <height> -output <image.png> -depth <depth_min> <depth_max> <depth_image.png> [-normals <normals_image.png>]\n";
		return 1;
	}

	srand(static_cast <unsigned> (time(0)));

	ArgParser parser(argc, argv);
	Renderer renderer(parser);
	renderer.render();
}