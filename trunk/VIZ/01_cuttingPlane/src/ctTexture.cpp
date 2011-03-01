#include "ctTexture.h"

CTtexture::CTtexture(){
	data = NULL;
}

CTtexture::~CTtexture(){
	if (data){
		delete [] data;
	}
}
void CTtexture::init(int _width, int _height, int _channels, const char * cmFilename, const char * ctFilename, int ctCnt, int ctX, int ctY, int ctZ){
	width	= _width;
	height	= _height;
	channels = _channels;
	data	= new float[width*height*channels];

	cm.loadFromFile(cmFilename);
	ct.loadFromFiles(ctFilename, ctCnt, ctX, ctY, ctZ);
}

float * CTtexture::getData(){
	return data;
}

void CTtexture::makeView(v3 normal, float z){
	v3 u(1.0, 0.0, 0.0);
	v3 v(0.0, 1.0, 0.0);
	v3 n(0.0, 0.0, 1.0);

	// rotate coord system 
	angle	= normal.angleTo(n);
	axis	= normal.getNormalized().cross(n);
	n = normal.getNormalized();
	u.rotate(angle, axis);
	v.rotate(angle, axis);

	int s,t;		// coord in plane space
	v3 pos;			// coord in 3d space
	v3 dataCenter = ct.getCenterPoint();
	v3 offset     = n*z;
	float rescaleFactor = 256/float(width);
	float value;
	v4 color;

	int w2 = width/2, h2= height/2;
	for (t = 0; t < height; t++){
		for (s = 0; s < width; s++){
			// calculate position in 3d
			pos = u*((s-w2)*rescaleFactor) + v*((t-h2)*rescaleFactor);
			pos += offset + dataCenter;
			/*if (pos.x>=168.f && pos.y>=176){
				printf("?\n");
			}*/
			// get value at this position
			value = ct.getValueAt(pos);
			// get color for the value
			color = cm.mapValueToColor(value);
			// scale color from range 0-255 to 0.0-1.0
			color = color/255.f;

			//if (color.y>0.5f){
			//	printf("JOOO\n");
			//}

			// write into texture data...
			data[channels*t*width+channels*s + 0] = color.x;
			data[channels*t*width+channels*s + 1] = color.y;
			data[channels*t*width+channels*s + 2] = color.z;
			data[channels*t*width+channels*s + 3] = color.w;
		}
	}

	
}
