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

bool BBox::intersect(Ray *pRay)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin  = (bounds[ pRay->sign[0]    ].x - pRay->org.x) * pRay->inv_dir.x;
	tmax  = (bounds[ 1 - pRay->sign[0]].x - pRay->org.x) * pRay->inv_dir.x;
	tymin = (bounds[ pRay->sign[1]    ].y - pRay->org.y) * pRay->inv_dir.y;
	tymax = (bounds[ 1 - pRay->sign[1]].y - pRay->org.y) * pRay->inv_dir.y;
	if ( (tmin > tymax) || (tymin > tmax) )
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds[ pRay->sign[2]     ].z - pRay->org.z) * pRay->inv_dir.z;
	tzmax = (bounds[ 1 - pRay->sign[2] ].z - pRay->org.z) * pRay->inv_dir.z;
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	if ((tmin < pRay->max) && (tmax > pRay->min)){
		pRay->min = min2f(tmin, tmax);
		pRay->max = max2f(tmin, tmax);
		return true;
	}
	return false;
}
	
