#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int numParticles);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	int get_total_num_states();

	

private:
	//list of springs
	//index i is spring_i return a list of all particles that i is attatched to 
	vector<vector<int>> spring_list;
	//store the numbe of springs for each type for each particle
	// i==0 : nb of structural springs
	// i==1 : nb of shear springs
	// i==2 : nb of flex springs
	vector<vector<int>> nb_of_springs;

	Vector3f anim_vec = Vector3f(2.f, 0.f, 0.f);
	


	const float G = 9.8f;
	const float m = 0.02f;
	const float drag = 0.03f;
	const float spring_cste = 5.f;
	const float rest_l = 0.35f;

	//convert the grid index into linear index
	int state_indexOf(int i, int j) { return 2* (m_numParticles * i + j); }
	int indexOf(int i, int j) { return m_numParticles * i + j; }
	void render_spring(int i, int j);
	void add_structural_springs(vector<int> &list , int i, int j, vector<int>& counter);
	void add_shear_springs(vector<int>& list, int i, int j, vector<int>& counter);
	void add_flex_springs(vector<int>& list, int i, int j, vector<int>& counter);
	Vector3f calculate_structural_spring_force(int i, int j, vector<Vector3f> state);
	Vector3f calculate_shear_spring_force(int i, int j, vector<Vector3f> state);
	Vector3f calculate_flex_spring_force(int i, int j, vector<Vector3f> state);
};


#endif
