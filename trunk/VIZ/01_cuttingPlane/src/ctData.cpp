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
	return v3(scX*float(dimX)/2, scY*float(dimY)/2,scZ*float(dimZ)/2);
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

	szX = dimX * scX;
	szY = dimY * scY;
	szZ = dimZ * scZ;
	// backspace...
	BACKSPACE(chars);
	printf("LOADING CT images (%i) DONE\n", cnt);
	return true;
}
