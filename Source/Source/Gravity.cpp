#include "Gravity.h"

Gravity::Gravity(Particle *p) :
  m_p(p) 
{
	gravityForce = Vec2f(0, -9.81 * 0.001);
}

void Gravity::apply()
{
	m_p -> m_Force += gravityForce;
	
}

void Gravity::draw()
{
  glBegin( GL_LINES );
  glColor3f(0.6, 0.2, 0.8);
  glVertex2f( m_p->m_Position[0], m_p->m_Position[1] );
  glColor3f(0.6, 0.2, 0.8);
  glVertex2f( m_p->m_Position[0] - gravityForce[0], m_p->m_Position[1] + gravityForce[1] );
  glEnd();
}

Gravity::~Gravity(void)
{
}
