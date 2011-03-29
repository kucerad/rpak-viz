#include "Tetrahedron.h"


Tetrahedron::Tetrahedron(v3 &a, v3 &b, v3 &c, v3 &d)
{
	positions[0] = a;
	positions[1] = b;
	positions[2] = c;
	positions[3] = d;
	colors[0] = v3(1.0, 0.0, 0.0);
	colors[1] = v3(0.0, 1.0, 0.0);
	colors[2] = v3(0.0, 0.0, 1.0);
	colors[3] = v3(1.0, 1.0, 1.0);
}


Tetrahedron::~Tetrahedron(void)
{
}

void Tetrahedron::draw()
{
	int k;
	glBegin(GL_LINES);
		for (int i=0; i<12; i++){
			k = drawIndices[i];
			glColor3f( colors[k].r, colors[k].g, colors[k].b );
			glVertex3f( positions[k].x, positions[k].y, positions[k].z );
		}
	glEnd();

}