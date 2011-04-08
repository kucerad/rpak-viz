#ifndef _BBOX_H
#define _BBOX_H

#include "Vector3.h"
#include "Ray.h"
#include "utils.h"

class BBox
{
public:
	BBox();
	BBox(v3 _min, v3 _max);
	~BBox(void);

	bool intersect(Ray &rRay);
	//v3 min, max;
	v3 bounds[2];
};

#endif
