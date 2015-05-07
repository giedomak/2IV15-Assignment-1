#include "SpringForce.h"
#include <iostream>
using namespace std;

SpringForce::SpringForce(Particle *p1, Particle * p2, double dist, double ks, double kd) :
	m_p1(p1), m_p2(p2), m_dist(dist), m_ks(ks), m_kd(kd) {}

void SpringForce::apply()
{
	//vector l is the first position minus the second position
	Vec2f l = (m_p1->m_Position - m_p2->m_Position);
	//the length of the vector L, which can be calculated by taking the root of the dot product, * is dot product
	float l_length = sqrt(l * l);
	/*
		variable defitions:
		m_ks is the springs constant
		m_dist is the distance between the 2 points, this is used to determine the force 
		m_kd is the damping constant
		m_Velocity of m_p1 and m_p2 is the velocity
		
	*/
	Vec2f f = (m_ks * (l_length - m_dist) + m_kd * (((m_p1->m_Velocity - m_p2->m_Velocity) * l) / l_length)) * (l / l_length);

	//move vector 1 towards vector 2 and 2 towards vector 1
	//m_p1->m_Force -= f; //this moves point 1
	m_p2->m_Force += f; //this moves point 2
}

void SpringForce::draw()
{
	
	// Draw forces
	glBegin( GL_LINES );
	glColor3f(0.6, 0.2, 0.8);
	glVertex2f( m_p1->m_Position[0], m_p1->m_Position[1] );
	glColor3f(0.6, 0.2, 0.8);
	glVertex2f(  m_p2->m_Position[0] , m_p2->m_Position[1]  );
	glEnd();
}
