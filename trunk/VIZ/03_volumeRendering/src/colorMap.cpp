#include "colorMap.h"

ColorMap::ColorMap(){

}

ColorMap::~ColorMap(){
	
}

void ColorMap::loadFromFile(const char * filename){
	//printf("LOADING Color Map ");
	cm.clear();
	cm.load(filename);
	width = cm.width;
	//printf("(width = %i). DONE\n", width);
	
}
	
//		get color for value [0.0 - 1.0]
v4 ColorMap::mapValueToColor(const float value){
	if (value<=BACKGROUND_VALUE){
		return BACKGROUND_COLOR;
	}
	int	  x;
	float t;
	getCoordsForInterpolation(value*(width-1), 1.0f, x,t);
	if (x>=(width-1)){
		return cm.getPixel(width-1,0);
	}
	v4 valA = cm.getPixel(x,0);
	v4 valB = cm.getPixel(x+1,0);

	return interpolate1DlinVector4(valA, valB, t)/255.0;
} 
