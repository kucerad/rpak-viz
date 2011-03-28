#ifndef _CTTEXTURE_H
#define _CTTEXTURE_H

#include "ctData.h"
#include "colorMap.h"
#include "Vector4.h"
#include <vector>

//extern float rotAngle;

using namespace std;

class CTtexture{
public:
	CTtexture();
	~CTtexture();
	void init(int _width, int _height, int _channels, const char * cmFilename, const char * ctFilename, int ctCnt, int ctX, int ctY, int ctZ);
	void changeCm(const char * cmFilename);
	void makeView(v3 normal, float z);
	float * getData();

	void getBox(v3 &min, v3 &max);

	int width, height;
	v3 axis;
	float angle;
	ColorMap cm;
private:
	int			channels;
	CTdata		ct;
	float		* data;

	vector<v3>	intersections;	
	void		calcIntersections(const v3 &normal, const v3 &u, const v3 &v, const float z);
	bool		intersect(const v3 &l0, const v3 &p0, const v3 &normal, const v3 &line, float max, const v3 &u, const v3 &v, v3 &intersection);
};

#endif