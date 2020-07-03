#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{	
	//get state
	vector<Vector3f> X = particleSystem->getState();
	//get derivative of the system at state
	vector<Vector3f> f = particleSystem->evalF(X);
	
	//calculate new state;
	if(f.size()>= 1)
	for (unsigned int  i = 0; i < particleSystem->get_total_num_states(); i++)
	{
		X[i] = X[i] + (stepSize * f[i]);
	}

	//std::cout << "Current state: " << X[0].x() << ", " << X[0].y() << ", " << X[0].z() << endl;
	particleSystem->setState(X);

}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)

{	

	//get state
	vector<Vector3f> X1 = particleSystem->getState();
	//get derivative of the system at state
	vector<Vector3f> f1 = particleSystem->evalF(X1);
	if (f1.size() >= 1) {
		//calculate new state
		vector<Vector3f> X2;
		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{
			X2.push_back(X1[i] + (stepSize * f1[i]));
		}
		// get derivative of the system at state
		vector<Vector3f> f2 = particleSystem->evalF(X2);
		//calculate new state
		vector<Vector3f> X3;
		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{
			X3.push_back(X1[i] + (stepSize * f2[i]));
		}

		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{
			X1[i] = X1[i] + (stepSize / 2.0f) * (f1[i] + f2[i]);
		}

	}
	//std::cout << "Current state: " << X[0].x() << ", " << X[0].y() << ", " << X[0].z() << endl;
	particleSystem->setState(X1);
}


/* RK4 ODE Solver
	output_t f1 = f(t, X);
	output_t f2 = f(t + time_step / 2.0, X + time_step * f1 / 2.0);
	output_t f3 = f(t + time_step / 2.0, X + time_step * f2 / 2.0);
	output_t f4 = f(t + time_step, X + time_step * f3);
*/
void RK4::takeStep(ParticleSystem* particleSystem, float stepSize)
{	

	//get state
	vector<Vector3f> X1 = particleSystem->getState();
	//get derivative of the system at state
	vector<Vector3f> f1 = particleSystem->evalF(X1);
	if (f1.size() >= 1) {
		//calculate new state
		vector<Vector3f> X2;
		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{	
			X2.push_back(X1[i] + (stepSize * f1[i] / 2.0));
		}
		// get derivative of the system at state
		vector<Vector3f> f2 = particleSystem->evalF(X2);
		//calculate new state
		vector<Vector3f> X3;
		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{
			X3.push_back(X1[i] + (stepSize * f2[i] / 2.0));
		}
		// get derivative of the system at state
		vector<Vector3f> f3 = particleSystem->evalF(X3);
		//calculate new state
		vector<Vector3f> X4;
		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{
			X4.push_back(X1[i] + (stepSize * f3[i]));
		}
		// get derivative of the system at state
		vector<Vector3f> f4 = particleSystem->evalF(X4);

		//calculate new state - RK4;
		for (unsigned int i = 0; i < particleSystem->get_total_num_states(); i++)
		{
			X1[i] = X1[i] + stepSize * ((f1[i] + f2[i] * 2 + f3[i] * 2 + f4[i]) / 6);
		}

	}

	//std::cout << "Current state: " << X[0].x() << ", " << X[0].y() << ", " << X[0].z() << endl;
	particleSystem->setState(X1);
}