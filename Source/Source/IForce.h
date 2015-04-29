#pragma once

#include <glut.h>

class IForce
{
 public:
	 virtual void apply() = 0;
	 virtual void draw() = 0;
};
