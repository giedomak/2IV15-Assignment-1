#include "SpringForce.h"
#include <iostream>
using namespace std;

SpringForce::SpringForce(Particle *p1, Particle * p2, double dist, double ks, double kd) :
	m_p1(p1), m_p2(p2), m_dist(dist), m_ks(ks), m_kd(kd) {}

void SpringForce::apply()
{
	Vec2f l = (m_p1->m_Position - m_p2->m_Position);
	float l_length = sqrt(l * l);

	Vec2f f = (m_ks * (l_length - m_dist) + m_kd * (((m_p1->m_Velocity - m_p2->m_Velocity) * l) / l_length)) * (l / l_length);
	m_p1->m_Force -= f;
	m_p2->m_Force += f;
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
