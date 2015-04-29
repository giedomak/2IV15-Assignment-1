#pragma once

#include "Particle.h"
#include "iforce.h"

class Gravity :
	public IForce
{
public:
	Gravity(Particle *p);

	Gravity(Particle *p, Vec2f force);

	virtual void apply();

	virtual void draw();

	~Gravity(void);

private:
	Vec2f gravityForce;
	Particle * const m_p;
};

