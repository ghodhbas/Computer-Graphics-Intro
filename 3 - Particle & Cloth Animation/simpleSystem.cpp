
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem()
{
	//create one particle
	m_vVecState.push_back(Vector3f(1.f, 0.f, 0.f));
	m_vVecState.push_back(Vector3f(0.f, 0.f, 0.f));
	m_numParticles = 1;
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	// YOUR CODE HERE
	for (unsigned int  i = 0; i < state.size(); i++)
	{
		//get velocity
		f.push_back(Vector3f(-state[i].y(), state[i].x(), 0.f));
		f.push_back(Vector3f(0.f,0.f,0.f));
	}
	
	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{		
	for (unsigned int  i = 0; i < m_numParticles; i++)
	{
		Vector3f pos = getState()[i];;//YOUR PARTICLE POSITION
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glutSolidSphere(0.1f, 10.0f, 10.0f);
		glPopMatrix();
	}
		
}

int  SimpleSystem::get_total_num_states() {
	return 2* m_numParticles;
}
