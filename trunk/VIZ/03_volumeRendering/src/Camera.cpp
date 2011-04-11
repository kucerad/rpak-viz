#include "Camera.h"

Camera::Camera(v3 &_position, v3 &_up, v3 &_direction, int _width, int _height):
	position(_position),
	up(_up),
	direction(_direction),
	width(_width),
	height(_height),
	progress(0.0)
{
	right = up.cross(direction);
	imageData = NULL;
}
Camera::~Camera(void)
{
	SAFE_DELETE_ARRAY_PTR(imageData);
}

void Camera::snapShot(CTdata *ctData, int channels)
{
	int chs = printf("Rendering... ");
	// cast rays, trace them and make image
	int x,y,ch;
	int height2 = height/2;
	int width2 = width/2;
	int area = width*height;
	int chars = 0;
	SAFE_DELETE_ARRAY_PTR(imageData);
	imageData = new float[area*channels];
	v3 org;
	for (y=0; y<height; y++){
		for (x=0; x<width; x++){
			// each pixel of image
			org = position + up*-(y-height2) + right*-(x-width2);
			// create ray
			Ray vRay(org, direction);
			// send it through volumetric data and register color
			ctData->colorizeRay(&vRay);
			// colorize the proper pixel in image
			for (ch=0; ch<channels; ch++){
				imageData[(y*width + x)*channels + ch]=vRay.color.data[ch];
			}
			progress = float(y*width+x)/float(area);
			BACK(chars);
			chars = printf("( progress: %3.1f %% )", progress*100);
		}
	}
	BACKSPACE(chs);
	printf("Rendering done.\n");
}
