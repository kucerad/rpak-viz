#ifndef _VERTEX_H
#define _VERTEX_H
#include "Vector3.h"
#include "Vector4.h"

class Vertex{
public:
	Vertex(){
		isValid=false;
	}
	// copy constructor
	Vertex(Vertex &v){
		isValid		= v.isValid;
		normal		= v.normal;
		position	= v.position;
		color		= v.color;
		value		= v.value;
	}
	~Vertex(){}
	v3		normal;
	v3		position;
	v4		color;

	float	value;
	bool	isValid;
};

#endif
