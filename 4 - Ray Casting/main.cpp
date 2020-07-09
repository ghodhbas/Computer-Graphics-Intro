#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>


#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include "Renderer.h"

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);




#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  

    // Report help usage if no args specified.
    if (argc == 1) {
        std::cout << "Usage: a4 "
            << "-input <scene> -size <width> <height> -output <image.png> -depth <depth_min> <depth_max> <depth_image.png> [-normals <normals_image.png>]\n";
        return 1;
    }

    std::string sceneFilename;
    int width = 0;
    int height = 0;
    std::string outputFilename;
    std::string depthFilename;
    float minDepth = 0.0f;
    float maxDepth = 0.0f;
    std::string normalFilename;

    // Fill in your implementation here.
    // Parse arguments.
    int i = 1;
    while (i < argc) {
        if (!strcmp(argv[i], "-input")) {
            sceneFilename = argv[i + 1];
            i += 2;
        }
        else if (!strcmp(argv[i], "-size")) {
            width = std::atoi(argv[i + 1]);
            height = std::atoi(argv[i + 2]);
            i += 3;
        }
        else if (!strcmp(argv[i], "-output")) {
            outputFilename = argv[i + 1];
            i += 2;
        }
        else if (!strcmp(argv[i], "-depth")) {
            minDepth = (float)std::atof(argv[i + 1]);
            maxDepth = (float)std::atof(argv[i + 2]);
            depthFilename = argv[i + 3];
            i += 4;
        }
        else if (!strcmp(argv[i], "-normals")) {
            normalFilename = argv[i + 1];
            i += 2;
        }
        else {
            std::cout << "Usage: a4 "
                << "-input <scene> -size <width> <height> -output <image.png> -depth <depth_min> <depth_max> <depth_image.png> [-normals <normals_image.png>]\n";
            return 1;
        }
    }
    

   Renderer renderer(sceneFilename.c_str());
   renderer.Render(width, height,
        outputFilename,
        minDepth, maxDepth,
        depthFilename,
        normalFilename);
 
  ///TODO: below demonstrates how to use the provided Image class
  ///Should be removed when you start
  //Vector3f pixelColor (1.0f,0,0);
  ////width and height
  //Image image( 10 , 15 );
  //image.SetPixel( 5,5, pixelColor );
  //image.SaveImage("TEEEEST.bmp");
  //return 0;
}

