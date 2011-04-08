#include "BBox.h"


BBox::BBox(v3 _min, v3 _max)
{
	bounds[0] = _min;
	bounds[1] = _max;
}
BBox::BBox()
{
	bounds[0] = v3(0,0,0);
	bounds[1] = v3(1,1,1);
}


BBox::~BBox(void)
{
}

bool BBox::intersect(Ray &rRay)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin  = (bounds[ rRay.sign[0]    ].x - rRay.org.x) * rRay.inv_dir.x;
	tmax  = (bounds[ 1 - rRay.sign[0]].x - rRay.org.x) * rRay.inv_dir.x;
	tymin = (bounds[ rRay.sign[1]    ].y - rRay.org.y) * rRay.inv_dir.y;
	tymax = (bounds[ 1 - rRay.sign[1]].y - rRay.org.y) * rRay.inv_dir.y;
	if ( (tmin > tymax) || (tymin > tmax) )
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds[ rRay.sign[2]     ].z - rRay.org.z) * rRay.inv_dir.z;
	tzmax = (bounds[ 1 - rRay.sign[2] ].z - rRay.org.z) * rRay.inv_dir.z;
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	if ((tmin < rRay.max) && (tmax > rRay.min)){
		rRay.min = min2f(tmin, tmax);
		rRay.max = max2f(tmin, tmax);
		return true;
	}
	return false;
}
	
