#include "ctData.h"

CTdata::CTdata(){
	data = NULL;
}

CTdata::~CTdata(){
	if (data){
		delete [] data;
	}
}

v3 CTdata::getCenterPoint(){
	return v3(float(dimX)/2, float(dimY)/2,float(dimZ)/2);
}

bool CTdata::loadFromFiles(const char * filename, int cnt, int scaleX, int scaleY, int scaleZ){
	int chars = 0;
	scX = scaleX;
	scY = scaleY;
	scZ = scaleZ;
	char filen [50];
	unsigned char * rawData;
	PNG ctfile;
	int width=0, height=0; 
	int x,y,z;
	float val;
	for(z=0; z<cnt; z++){
		sprintf(filen, filename, z+1);
		ctfile.clear();
		if (!ctfile.load(filen)){
			return false;
		}
		rawData		= ctfile.getData();
		height		= ctfile.height;
		width		= ctfile.width;
		if (data==NULL){
			data = new float[cnt*width*height];
		}
		for (y=0; y<height; y++){
			for (x=0; x<width; x++){
				// 2x 8-bit to 1x 16-bit
				val = 256*rawData[2*y*width + 2*x]+rawData[2*y*width + 2*x + 1];
				// save in array
				data[z*width*height + y*width +x] = val;
			}
		}
		BACKSPACE(chars);
		chars = printf("LOADING CT images: %03i %%",((z+1)*100)/cnt);
	
	}
	dimX = width;
	dimY = height;
	dimZ = cnt;

	// backspace...
	BACKSPACE(chars);
	printf("LOADING CT images (%i) DONE\n", cnt);
	return true;
}


float CTdata::getValueAt(int x, int y, int z, bool &inFlag){
	if (x<0 || y<0 || z<0 || x>dimX || y>dimY || z>dimZ){
		// out of grid
		return 0.0f;
	}
	inFlag = true;
	return data[ z*dimX*dimY + y*dimX + x ];
}

float CTdata::getValueAt(const v3 position){
	return getValueAt(position.x, position.y, position.z);
}

float CTdata::getValueAt(const float x, const float y, const float z){
	// get nearby coordinates
	int xc, yc, zc;
	float xt, yt,zt;
	float val1, val2, val3, val4, val5, val6, val7, val8;
	getCoordsForInterpolation(x, int(scX), xc, xt);
	getCoordsForInterpolation(y, int(scY), yc, yt);
	getCoordsForInterpolation(z, int(scZ), zc, zt); 

	// get values
	bool in = false;
	val1 = getValueAt(xc  , yc  , zc  , in);
	val2 = getValueAt(xc  , yc+1, zc  , in);
	val3 = getValueAt(xc  , yc  , zc+1, in);
	val4 = getValueAt(xc  , yc+1, zc+1, in);
	val5 = getValueAt(xc+1, yc  , zc+1, in);
	val6 = getValueAt(xc+1, yc+1, zc+1, in);
	val7 = getValueAt(xc+1, yc  , zc  , in);
	val8 = getValueAt(xc+1, yc+1, zc  , in);
	float val = 0.0f;
	if (in){
		// at least one point is inside 3D grid... 
		// interpolate then
		val = interpolate3Dlin(val1, val2, val3, val4, val5, val6, val7, val8, xt, yt, zt);
	} else {
		return BACKGROUND_VALUE;
	}

	return val/3272;
}
