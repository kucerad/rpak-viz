#ifndef _CAMERA_H
#define _CAMERA_H

#include "Ray.h"
#include "ctData.h"
#include "colorMap.h"
#include "Shader.h"


class Camera
{
public:
	Camera(v3 &_position, v3 &_up, v3 &_direction, int _width, int _height);
	~Camera(void);
	void snapShot(CTdata *ctData, int channels);
	float * imageData;
	v3 position;
	v3 up, right, direction;
	float progress;
private:
	int width, height;	
	
};

#endif
