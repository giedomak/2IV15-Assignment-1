#pragma once

#include "Particle.h"
#include "IForce.h"

class CircularWireConstraint {
public:
	CircularWireConstraint(Particle *p, const Vec2f & center, const double radius);

	virtual void apply();

	virtual void draw();

private:

	Particle * const m_p;
	Vec2f const m_center;
	double const m_radius;
};
