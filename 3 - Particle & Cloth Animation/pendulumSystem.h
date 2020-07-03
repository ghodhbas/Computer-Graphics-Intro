#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <iostream>
#include < cstdlib >
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	void draw();
	int get_total_num_states();


private:

	const float G = 9.8f;
	const float m = 0.05f;
	const float drag = 0.01f;
	const float spring_cste = 1.8f;
	const float rest_l = 0.7f;

	Vector3f calculate_spring_force(int i, int j, vector<Vector3f> state);
	
};

#endif
