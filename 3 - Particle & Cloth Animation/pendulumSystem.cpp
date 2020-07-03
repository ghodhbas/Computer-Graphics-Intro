
#include "pendulumSystem.h"

/*
							even index = position
							   odd index = v
*/
PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	
	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < 2* m_numParticles; i+=2) {
		
		// for this system, we care about the position and the velocity
		//position
		//if (i == 0) {
		//	m_vVecState.push_back(Vector3f(0.f, 0.f, 0.f));
		//}
		//else if (i == 2 * m_numParticles - 1) {
		//	m_vVecState.push_back(m_vVecState[i - 2] + Vector3f(0.f, -1.f, 0.f));
		//}
		//else {
		//	//get postion of last particle and add a rest lengrth transition
		//	m_vVecState.push_back(m_vVecState[i - 2] + Vector3f(0.f, -1 * rest_l, 0.f));
		//}
		////initial velocity
		//m_vVecState.push_back(Vector3f(0.f, 0.f, 0.f));


		if (i == 0) {
			m_vVecState.push_back(Vector3f(0.f, 0.f, 0.f));
		}
		else if (i == 2 * m_numParticles - 1) {
			m_vVecState.push_back(m_vVecState[i - 2] + Vector3f(-1.f, 0.f, 0.f));
		}
		else {
			//get postion of last particle and add a rest lengrth transition
			m_vVecState.push_back(m_vVecState[i - 2] + Vector3f(-1 * rest_l,0.f , 0.f));
		}
		//initial velocity
		m_vVecState.push_back(Vector3f(0.f, 0.f, 0.f));

	}
	
}


Vector3f PendulumSystem::calculate_spring_force(int i, int j, vector<Vector3f> state) {
	float d = (state[i] - state[j]).abs();
	Vector3f d_dir = (state[i] - state[j]).normalized();
	return -spring_cste * (d - rest_l) * d_dir;

}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
/* returns array of size 2*n.  even index = v
							   odd index = F
*/
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	
	for (unsigned int i = 0; i < 2* m_numParticles; i +=2)
	{
		//save velocity
		f.push_back(state[i+1]);

		//CALCULATE FORCES --> acceleration
		// for this system, we care about the position and the velocity
		if (i == 0) {
			//particle at origin doens't move
			f.push_back(Vector3f(0.f, 0.f, 0.f));
		}
		else {

			Vector3f force = Vector3f(0.f, 0.f, 0.f);
			//gravity
			force += Vector3f(0.f, -m * G, 0.f);
			//drag  --> -k * v_i
			force += -drag * f[i];
			//spring
			force += calculate_spring_force(i, i - 2, state); 
			//react spring force
			if(i+2< 2 * m_numParticles) force += calculate_spring_force(i, i + 2, state);
			f.push_back(force/m);
		}

	}


	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < 2*m_numParticles; i+=2) {
		Vector3f pos  = m_vVecState[i];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.1f,10.0f,10.0f);
		glPopMatrix();
	}


	//draw springs
	glLineWidth(50.f);
	GLfloat springColor[] = { 1.f, 1.f, 1.f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, springColor);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 2 * m_numParticles; i += 2) {
		glVertex3f(m_vVecState[i][0], m_vVecState[i][1], m_vVecState[i][2]);
	}
	glEnd();
}


int  PendulumSystem::get_total_num_states() {
	return 2* m_numParticles;
}