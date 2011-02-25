//-----------------------------------------------------------------------------
//  [GSY] Utilities - common functions
//  15/03/2008
//-----------------------------------------------------------------------------
//  Controls: 
//-----------------------------------------------------------------------------


#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

#define M_PI 3.1415926535f
enum eAXIS {
	X,
	Y,
	Z
};


float interpolate1Dlin(const float valX, const float valY, const float t);

float interpolate3Dlin(const float val1, const float val2, const float val3, const float val4,const float val5, const float val6,const float val7, const float val8, const float x, const float y, const float z);

#endif __UTILS_H__
