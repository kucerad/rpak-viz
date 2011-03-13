#ifndef _CTTEXTURE_H
#define _CTTEXTURE_H

#include "ctData.h"
#include "colorMap.h"
#include "Vector4.h"

class CTtexture{
public:
	CTtexture();
	~CTtexture();
	void init(int _width, int _height, int _channels, const char * cmFilename, const char * ctFilename, int ctCnt, int ctX, int ctY, int ctZ);
	void changeCm(const char * cmFilename);
	void makeView(v3 normal, float z);
	float * getData();

	int width, height;
	v3 axis;
	float angle;
	ColorMap cm;
private:
	int channels;
	CTdata ct;
	float * data;

};

#endif