#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include "Vector4.h"

#define M_PI 3.1415926535f
enum eAXIS {
	X,
	Y,
	Z
};


Vector4	  interpolate1DlinVector4(const Vector4& vecA, const Vector4& vecB, const float t);

float interpolate1Dlin(const float valX, const float valY, const float t);

float interpolate3Dlin(const float val1, const float val2, const float val3, const float val4,const float val5, const float val6,const float val7, const float val8, const float x, const float y, const float z);

void getCoordsForInterpolation(const float realCoord, const int axisScale, int &lowerCoord, float &t);

#endif __UTILS_H__
