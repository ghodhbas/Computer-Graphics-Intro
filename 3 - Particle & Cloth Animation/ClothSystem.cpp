#include "ClothSystem.h"
#include <iostream>



//TODO: Initialize here
ClothSystem::ClothSystem(int numParticles) :ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	//create particles in a grid
	for (int i = 0; i < m_numParticles; i++)
	{
		for (int j = 0; j < m_numParticles; j++)
		{

			//get postion of last particle and add a rest lengrth transition
			m_vVecState.push_back(Vector3f(i * 1.f * rest_l,
				0.f,
				j * -1.f * rest_l));

			//initial velocity
			m_vVecState.push_back(Vector3f(0.f, 0.f, 0.f));

		}
	}

	//build data structure to allow tracking of springs for physics and rendering
	for (int i = 0; i < m_numParticles; i++)
	{	
		for (int j = 0; j < m_numParticles; j++)
		{	
			vector<int> list;
			vector<int> nb_spring{ 0,0,0 };
			//structural
			add_structural_springs(list, i, j, nb_spring);
			//shear springs
			add_shear_springs(list, i, j, nb_spring);
			//flex springs
			add_flex_springs(list, i, j, nb_spring);
			//add to list
			spring_list.push_back(list);
			nb_of_springs.push_back(nb_spring);
		}
	}

}

void ClothSystem::add_structural_springs(vector<int>& list, int i, int j, vector<int>& counter) {
	//left
	if ((j - 1) >= 0) {
		list.push_back(state_indexOf(i, j - 1));
		counter[0]++;
	}
	//right
	if ((j + 1) < m_numParticles) {
		list.push_back(state_indexOf(i, j + 1));
		counter[0]++;
	}
	//up
	if ((i - 1) >= 0) {
		list.push_back(state_indexOf(i - 1, j));
		counter[0]++;
	}
	//down
	if ((i + 1) < m_numParticles) {
		list.push_back(state_indexOf(i + 1, j));
		counter[0]++;
	}

}

void ClothSystem::add_shear_springs(vector<int>& list, int i, int j, vector<int>& counter) {
	//up left
	if ((j - 1) >= 0 && (i - 1) >= 0) {
		list.push_back(state_indexOf(i - 1, j - 1));
		counter[1]++;
	}
	//up right
	if ((i - 1) >= 0 && (j + 1) < m_numParticles){
		list.push_back(state_indexOf(i-1, j + 1));
		counter[1]++;
	}
	//down left
	if ((i + 1) < m_numParticles && (j - 1) >= 0){
		list.push_back(state_indexOf(i + 1, j-1));
		counter[1]++;
	}
	//down right
	if ((i + 1) < m_numParticles && (j + 1) < m_numParticles){
		list.push_back(state_indexOf(i + 1, j+1));
		counter[1]++;
	}
}

void ClothSystem::add_flex_springs(vector<int>& list, int i, int j, vector<int>& counter) {
	//left
	if ((j - 2) >= 0){
		list.push_back(state_indexOf(i, j - 2));
		counter[2]++;
	}
	//right
	if ((j + 2) < m_numParticles){
		list.push_back(state_indexOf(i, j + 2));
		counter[2]++;
	}
	//up
	if ((i - 2) >= 0){
		list.push_back(state_indexOf(i - 2, j));
		counter[2]++;
	}
	//down
	if ((i + 2) < m_numParticles){
		list.push_back(state_indexOf(i + 2, j));
		counter[2]++;
	}
}



Vector3f ClothSystem::calculate_structural_spring_force(int i, int j, vector<Vector3f> state) {
	float d = (state[i] - state[j]).abs();
	Vector3f d_dir = (state[i] - state[j]).normalized();
	return -spring_cste * (d - rest_l) * d_dir;

}

//rest_l for shear is sqrt(2)*rest_l
Vector3f ClothSystem::calculate_shear_spring_force(int i, int j, vector<Vector3f> state) {
	float d = (state[i] - state[j]).abs();
	Vector3f d_dir = (state[i] - state[j]).normalized();
	return -spring_cste * (d -  ( ((float) sqrt(2))*rest_l)) * d_dir;

}

//rest_l for flex is2*rest_l
Vector3f ClothSystem::calculate_flex_spring_force(int i, int j, vector<Vector3f> state) {
	float d = (state[i] - state[j]).abs();
	Vector3f d_dir = (state[i] - state[j]).normalized();
	return -spring_cste * (d - (2.f * rest_l)) * d_dir;

}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{	
	vector<Vector3f> f;

	for (int i = 0; i < m_numParticles; i++)
	{
		for (int j = 0; j < m_numParticles; j++)
		{
			

			//fix top 2 corners and dont apply forces
			if (state_indexOf(i, j) == 0 || state_indexOf(i, j) == ((m_numParticles - 1) * 2)) {
				//save velocity
				if (this->animate) {
					//simple animation
					f.push_back(anim_vec);
					if (state[state_indexOf(i, j)].x() > 5.f  ) {
						anim_vec = Vector3f(-2.f, 0.f, 0.f);
					}

					if (state[state_indexOf(i, j)].x() < 0) {
						anim_vec = Vector3f(2.f, 0.f, 0.f);
					}

				}else {
					f.push_back(state[state_indexOf(i, j) + 1]);
				}
				
				f.push_back(Vector3f(0.f, 0.f, 0.f));
			}
			else {
				//save velocity
				f.push_back(state[state_indexOf(i, j) + 1]);
				//gravity
				Vector3f force = Vector3f(0.f, -m * G, 0.f);
				//drag  --> -k * v_i
				//force += -drag * f[state_indexOf(i,j)];
				force += -drag * f.back();

				//get list of attached springs to this particle
				vector<int> list = spring_list[indexOf(i, j)];
				//get number of springs of each type
				vector<int> nb_sp = nb_of_springs[indexOf(i, j)];
				int nb_struct = nb_sp[0];
				int nb_shear = nb_sp[1];
				int nb_flex = nb_sp[2];

					//go through structural springs and calculate their forces
				for (int k = 0; k < nb_struct; k++) {
						force += calculate_structural_spring_force(state_indexOf(i, j), list[k], state);
				}
					//go through shear springs and calculate their forces
				for (int k = nb_struct; k < (list.size() - nb_flex); k++) {
				
					force += calculate_shear_spring_force(state_indexOf(i, j), list[k], state);
				}
					//go through flex springs and calculate their forces
				for (int k = nb_shear + nb_struct ; k < list.size(); k++) {
					force += calculate_flex_spring_force(state_indexOf(i, j), list[k], state);
				}

				f.push_back(force / m);
			}
		}
	}

	
	return f;
}

void ClothSystem::render_spring(int i, int j) {
	//get list of springs for this index
	vector<int> list = spring_list[indexOf(i, j)];
	Vector3f v1 = m_vVecState[state_indexOf(i, j)];

	
	//draw lines - springs
	glLineWidth(5.f);

	vector<int> vec_nb_s = nb_of_springs[indexOf(i, j)];
	int nb_struct = vec_nb_s[0];
	int nb_shear = vec_nb_s[1];
	int nb_flex = vec_nb_s[2];

	//render all springs
	//for (int k = 0; k < list.size(); k++) {
	//render struct springs
	for (int k = 0; k < nb_struct; k++) {
	//render shear springs
	//for (int k = nb_struct; k < list.size() - nb_flex; k++) {
	//render flex springs
	//for (int k = nb_shear+nb_struct; k < list.size(); k++) {
		Vector3f v2 = m_vVecState[list[k]];
		glBegin(GL_LINE_STRIP);
		glColor4f(1, 1, 1, 1);
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
		glEnd();
	}
}


///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
	for ( int i = 0; i <  m_numParticles; i++)
	{
		for ( int j = 0; j < m_numParticles; j++)
		{

			//render particles
			int index = state_indexOf(i, j);
			Vector3f pos = m_vVecState[index];
			glPushMatrix();
			glTranslatef(pos[0], pos[1], pos[2]);
			glutSolidSphere(0.05f, 10.0f, 10.0f);
			glPopMatrix();

			//render spring
			//if (i == 3 && j == 2) render_spring(i, j);
			render_spring(i, j);
		}
	}

}

int  ClothSystem::get_total_num_states() {
	return 2 * (m_numParticles*m_numParticles);
}