#include "Camera.h"

Camera::Camera(v3 &_position, v3 &_up, v3 &_direction, int _width, int _height, float _scale):
	position(_position),
	up(_up),
	direction(_direction),
	width(_width),
	height(_height),
	progress(0.0),
	scale(_scale)
{
	right = up.cross(direction);
	imageData = NULL;
}
Camera::~Camera(void)
{
	SAFE_DELETE_ARRAY_PTR(imageData);
}
void Camera::printOut(){
	printf("Camera params:\nPosition: ");
	position.printOut();
	printf("\nUp: ");
	up.printOut();

	printf("\nRight: ");
	right.printOut();

	printf("\nDirection: ");
	direction.printOut();

	printf("\n");
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
	float progressPre = 0.f;
	v3 org;
	for (y=0; y<height; y++){
		for (x=0; x<width; x++){
			if (x == 372 && y == 200){
				int k = 0;
			}


			// each pixel of image
			org = position + up*-(y-height2)*scale + right*(x-width2)*scale;
			
			// create ray
			Ray vRay(org, direction);
			
			// send it through volumetric data and register color
			ctData->colorizeRay(&vRay);
			
			

			// colorize the proper pixel in image
			for (ch=0; ch<channels; ch++){
				imageData[(y*width + x)*channels + ch]=vRay.color.data[ch];
			}
			
			// update progress
			progress = float(y*width+x)/float(area);
			if (progress-progressPre>=0.01){
				BACK(chars);
				chars = printf("( progress: %3.1f %% | %i, %i )", progress*100, x,y);
				progressPre = progress;
			}
		}
	}
	// rendering done
	BACKSPACE(chs + chars);
	printf("Rendering done.\n");
}
