#include "Camera.h"

Camera::Camera(v3 &_position, v3 &_up, v3 &_direction, int _width, int _height):
	position(_position),
	up(_up),
	direction(_direction),
	right(up.cross(direction)),
	width(_width),
	height(_height),
	progress(0.0)
{

}
Camera::~Camera(void)
{

}

void Camera::snapShot(CTdata &ctData, ColorMap &colorMap, int channels)
{
	Shader phongShader();
	// cast rays, trace them and make image
	int x,y,ch;
	int height2 = height/2;
	int width2 = width/2;
	float area = width*height;
	v3 org;
	for (y=0; y<height; y++){
		for (x=0; x<width; x++){
			// each pixel of image
			org = position + up*(y-height2) + right*(x-height2);
			// create ray
			Ray vRay(org, direction);
			// send it through volumetric data and register color
			ctData.colorizeRay(&vRay, &colorMap, phongShader);
			// colorize the proper pixel in image
			for (ch=0; ch<channels; ch++){
				imageData[(y*width + x)*channels + ch]=vRay.color.data[ch];
			}
			progress = float(y*width+x)/area;
		}
	}
}
