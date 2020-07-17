#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include "SceneParser.h"
#include "RayTracer.h"
#include "ArgParser.h"

class Renderer
{
public:
	// Instantiates a renderer for the given scene.
	Renderer(const char* filename);

	~Renderer();

	// If depthFilename is empty, a depth image is not generated.
	void render(int width,
		int height,
		int max_bounces,
		const std::string& outputFilename,
		int shadows, int jitter);

	// Same for normalFilename and the normal image.
	void Render(int width,
		int height,
		int max_bounces,
		const std::string& outputFilename,
		float minDepth = 0.0f,
		float maxDepth = 0.0f,
		const std::string& depthFilename = "",
		const std::string& normalFilename = "");

private:
	SceneParser _sp;
};

#endif // RENDERER_H