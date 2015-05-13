#include "Particle.h"
#include "IForce.h"
#include "linearSolver.h"
#include <vector>

#define DAMP 0.98f
#define RAND (((rand()%2000)/1000.f)-1.f)
void simulation_step(std::vector<Particle*> pVector, std::vector<IForce*> forces, std::vector<IForce*> constraints, float dt)
{

	//implicitMatrixWithTrans * matrix = new implicitMatrixWithTrans();

	// fill matrix with c dot

	// solver.sole(matrix, some other parameters)

	// Loop over particles accumulate forces from solver

	// clear forces
	for (int i = 0; i < pVector.size(); i++) {
		pVector[i] -> m_Force[0] = 0;
		pVector[i] -> m_Force[1] = 0;
	}

	// Apply forces
	for(int i = 0; i < forces.size(); i++) {
		forces[i] -> apply();
	}

	// increase vel for each particle
	for (int i = 0; i < pVector.size(); i++) {
		pVector[i]->m_Velocity += pVector[i] -> m_Force * dt * dt;
	}

	// Displace particles with velocity
	for (int i = 0; i < pVector.size(); i++) {
		pVector[i] -> m_Position += (pVector[i] -> m_Velocity) * dt;
	}

	int ii, size = pVector.size();	
	for(ii=0; ii<size; ii++)
	{
		//pVector[ii]->m_Position += dt*pVector[ii]->m_Velocity;
		//pVector[ii]->m_Velocity = DAMP*pVector[ii]->m_Velocity + Vec2f(RAND,RAND) * 0.005;
	}
}

