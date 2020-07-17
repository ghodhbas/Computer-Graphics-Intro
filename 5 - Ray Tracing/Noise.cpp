#include "Noise.h"
#include "PerlinNoise.h"


Noise::Noise(int _octaves, const Vector3f& color1,
	const Vector3f& color2, float freq, float amp) :
	octaves(_octaves), frequency(freq), amplitude(amp) {
	color[0] = color1;
	color[1] = color2;
	init = true;
}

Noise::Noise(const Noise& n) :
	octaves(n.octaves), frequency(n.frequency),
	amplitude(n.amplitude), init(n.init)
{
	color[0] = n.color[0];
	color[1] = n.color[1];
}

bool Noise::valid()
{
	return init;
}

Noise::Noise() :
	octaves(0), init(false)
{}


Vector3f
Noise::getColor(const Vector3f& pos)
{
	// IMPLEMENT: Fill in this function ONLY.
	// Interpolate between two colors by weighted average
	double perlin = PerlinNoise::octaveNoise(pos, octaves);
	float m = std::sin(frequency * pos.x() + amplitude * perlin);
	float m_clamped = std::min(1.0f, std::max(m, 0.0f));

	return (float)m * color[0] + (1 - (float)m) * color[1];
}