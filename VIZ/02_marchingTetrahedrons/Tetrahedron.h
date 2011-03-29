#pragma once

#include "GLEW/glew.h"
#include "GL/glut.h"
#include "Vector3.h"

static int drawIndices[] = {0,1,0,2,0,3,1,2,2,3,3,1};


class Tetrahedron
{
public:
	Tetrahedron(v3 &a, v3 &b, v3 &c, v3 &d);
	~Tetrahedron(void);
	void draw();

	v3 positions[4];
	v3 colors[4];

};

