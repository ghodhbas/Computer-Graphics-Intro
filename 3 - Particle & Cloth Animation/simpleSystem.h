#ifndef SIMPLESYSTEM_H
#define SIMPLESYSTEM_H

#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include <vecmath.h>
#include <vector>

#include "particleSystem.h"


/* this system is a single particle rotating around the origin */

using namespace std;

class SimpleSystem: public ParticleSystem
{
public:
	SimpleSystem();
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	
	int get_total_num_states();

};

#endif
