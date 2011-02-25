#include "utils.h"

float interpolate1Dlin(const float valX, const float valY, const float t){
	if (t<=0.f) return valX;
	if (t>=1.f) return valY;
	return (1-t)*valX + t*valY;
}
float interpolate3Dlin(const float val1, const float val2, const float val3, const float val4,const float val5, const float val6,const float val7, const float val8, const float x, const float y, const float z){
	float v1 = interpolate1Dlin(val1, val2, y);
	float v2 = interpolate1Dlin(val3, val4, y);
	float v3 = interpolate1Dlin(val5, val6, y);
	float v4 = interpolate1Dlin(val7, val8, y);

	float v5 = interpolate1Dlin(v1, v2, z);
	float v6 = interpolate1Dlin(v4, v3, z);

	return interpolate1Dlin(v5, v6, x);
}