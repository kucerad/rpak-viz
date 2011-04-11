#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include "Vector4.h"
#include "Vertex.h"

#define M_PI 3.1415926535f
#define FLOAT_MAX FLT_MAX

enum eAXIS {
	X,
	Y,
	Z
};

#define BACKSPACE(cnt) for (int i=0; i<cnt; i++){printf("\b \b");}
#define BACK(cnt) for (int i=0; i<cnt; i++){printf("\b");}

#define SAFE_DELETE_PTR(VAR) if(VAR!=NULL){delete VAR; VAR=NULL;}
#define SAFE_DELETE_ARRAY_PTR(VAR) if(VAR!=NULL){delete [] VAR; VAR=NULL;}


__forceinline float min2f(float a, float b)
{
	return (a<b)?a:b;	
}
__forceinline float max2f(float a, float b)
{
	return (a>=b)?a:b;	
}
__forceinline float interpolate1Dlin(const float valX, const float valY, const float t){
	if (t<=0.f) return valX;
	if (t>=1.f) return valY;
	return (1-t)*valX + t*valY;
}
__forceinline Vertex interpolate1Dlin(Vertex* v1, Vertex* v2, const float t){
	if (t<=0.f) return *v1;
	if (t>=1.f) return *v2;
	Vertex out;
	out.value = (1-t)*v1->value + t * v2->value;
	out.normal = v1->normal * (1-t) + v2->normal * t;
	out.color = v1->color * (1-t) + v2->color * t;
	out.isValid = true;
	return out;
}
__forceinline v4	  interpolate1DlinVector4(const v4 & vecA, const v4 & vecB, const float t){
	float x = interpolate1Dlin( vecA.x, vecB.x, t);
	float y = interpolate1Dlin( vecA.y, vecB.y, t);
	float z = interpolate1Dlin( vecA.z, vecB.z, t);
	float w = interpolate1Dlin( vecA.w, vecB.w, t);
	return v4(x,y,z,w);
}



__forceinline float interpolate3Dlin(const float val1, const float val2, const float val3, const float val4,const float val5, const float val6,const float val7, const float val8, const float x, const float y, const float z){
	float v1 = interpolate1Dlin(val1, val2, y);
	float v2 = interpolate1Dlin(val3, val4, y);
	float v3 = interpolate1Dlin(val5, val6, y);
	float v4 = interpolate1Dlin(val7, val8, y);

	float v5 = interpolate1Dlin(v1, v2, z);
	float v6 = interpolate1Dlin(v4, v3, z);

	return interpolate1Dlin(v5, v6, x);
}
__forceinline Vertex interpolate3Dlin(Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4, Vertex* v5, Vertex* v6, Vertex* v7, Vertex* v8, float xt, float yt, float zt){
	Vertex ve1 = interpolate1Dlin(v1, v2, yt);
	Vertex ve2 = interpolate1Dlin(v3, v4, yt);
	Vertex ve3 = interpolate1Dlin(v5, v6, yt);
	Vertex ve4 = interpolate1Dlin(v7, v8, yt);

	Vertex ve5 = interpolate1Dlin(&ve1, &ve2, zt);
	Vertex ve6 = interpolate1Dlin(&ve4, &ve3, zt);

	return interpolate1Dlin(&ve5, &ve6, xt);
}



__forceinline void getCoordsForInterpolation(const float realCoord, const int axisScale, int &lowerCoord, float &t){
	lowerCoord	= int (realCoord/axisScale);
	t			= realCoord/axisScale - lowerCoord;
}
#endif __UTILS_H__
