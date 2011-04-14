#include "Ray.h"

Ray::Ray(v3 &_org, v3 &_dir):
	dir(_dir.getNormalized()),
	org(_org),
	color(v3(0.f, 0.f, 0.f)),
	min(-FLT_MAX),
	max(FLT_MAX)
{
	inv_dir = v3(1.0/dir.x, 1.0/dir.y, 1.0/dir.z);
	sign[0] = (inv_dir.x < 0);
	sign[1] = (inv_dir.y < 0);
	sign[2] = (inv_dir.z < 0);
}

Ray::~Ray()
{

}