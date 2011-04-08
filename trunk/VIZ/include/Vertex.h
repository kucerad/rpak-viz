#ifndef _VERTEX_H
#define _VERTEX_H
#include "Vector3.h"

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
		value		= v.value;
	}
	~Vertex(){}
	v3		normal;
	v3		position;
	float	value;
	bool	isValid;
};

#endif
