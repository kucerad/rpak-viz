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

void CTtexture::changeCm(const char * cmFilename) {
	cm.cm.clear();
	cm.loadFromFile(cmFilename);
}

float * CTtexture::getData(){
	return data;
}
bool CTtexture::intersect(const v3 &l0, const v3 &p0, const v3 &normal, const v3 &line, float max, const v3 &u, const v3 &v, v3 &intersection){
	// http://en.wikipedia.org/wiki/Line-plane_intersection
		
	float up = (p0 - l0).dot(normal);
	float dot = line.dot(normal);
	float d = -1.0f;
	if (dot<EPSILON && dot>-EPSILON){ // dot == 0 
		if (up<EPSILON && up>-EPSILON){ // up == 0			
			// line on plane
			// no intersections
		} else {
			// colinear...
			// skip - no intersection
		}
	} else {
		// intersection
		d = up/dot;
	}
	// intersection in proper range?
	if (d>=0.0f && d<=max){
		// true intersection
		// get position in plane coords...
		v3 p = line*d + l0;
		intersection.x = u.dot(p)*(500.0/256.0);
		intersection.y = v.dot(p)*(500.0/256.0);
		intersection.z = 0;
		return true;
	} // else {
		// false alarm!!!!
	//}
	return false;
}

void CTtexture::calcIntersections(const v3 &normal, const v3 &u, const v3 &v, const float _z){
	intersections.clear();
	v3 p0 = normal*_z;
	v3 p;
	float x(ct.szX),y(ct.szY),z(ct.szZ);
	float d=0, dot=0, up;
	float max;
	v3 intersection;
	v3 line;
	v3 halfCenter(ct.szX*0.5,ct.szY*0.5,ct.szZ*0.5);
	p0 = p0 + halfCenter;
	v3 l0;
	// for each of the 3 faces of data cube
		// X
		line = v3(1,0,0);
		max = ct.szX;
		// for each edge on the face
		l0 = v3(0,0,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(0,y,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(0,y,z);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(0,0,z);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
	
		// Y
		line = v3(0,1,0);
		max = ct.szY;
		// for each edge on the face
		l0 = v3(0,0,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(0,0,z);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(x,0,z);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(x,0,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}

		// Z
		line = v3(0,0,1);
		max = ct.szZ;
		// for each edge on the face
		l0 = v3(0,0,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(0,y,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(x,y,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		l0 = v3(x,0,0);
		if (intersect(l0, p0, normal, line, max, u, v, intersection)){
			intersections.push_back(intersection);	
		}
		// number of intersections?
		printf ("Intersections: %i\n", intersections.size());
}

void CTtexture::makeView(v3 normal, float z){
	v3 u(1.0, 0.0, 0.0);
	v3 v(0.0, 1.0, 0.0);
	v3 n(0.0, 0.0, 1.0);

	// rotate coord system 
	angle	= normal.angleTo(n);
	axis	= normal.cross(n).getNormalized();
	n.rotate(-angle, axis);
	u.rotate(-angle, axis);
	v.rotate(-angle, axis);
	
	u.rotate(DEGREES_TO_RADIANS(rotAngle), n);
	v.rotate(DEGREES_TO_RADIANS(rotAngle), n);

	calcIntersections(n, u, v, z);


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
			
			// get value at this position
			value = ct.getValueAt(pos);
			
			// get color for the value
			color = cm.mapValueToColor(value);
			
			// scale color from range 0-255 to 0.0-1.0
			color = color/255.f;

			// write into texture data...
			data[channels*t*width+channels*s + 0] = color.x;
			data[channels*t*width+channels*s + 1] = color.y;
			data[channels*t*width+channels*s + 2] = color.z;
			data[channels*t*width+channels*s + 3] = color.w;
		}
	}

	
}
