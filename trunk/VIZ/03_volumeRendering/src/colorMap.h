#ifndef _COLORMAP_H
#define _COLORMAP_H

#include "Vector4.h"	// alias v4
#include "utils.h"	// interpolation
#include "png.h"
#include "settings.h"
#include <vector>

using namespace std;

class ColorMap{
public:
	ColorMap();
	~ColorMap();
	// 
	void	loadFromFile(const char * filename);
	
	//		get color for value [0.0 - 1.0]
	v4		mapValueToColor(const float value); 

	PNG			cm;
private:
	int			width;
};


#endif