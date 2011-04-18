#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "GLEW/glew.h"
//#include "GLEE/glee.h"
#include "GL/glut.h"
#include "Vector4.h" // alias v4

static const float SAMPLE_DISTANCE = 0.7f;
static Vector4 BACKGROUND_COLOR(0.5f, 0.5f, 0.0f, 1.0f);
static const float BACKGROUND_VALUE = -1.0f;
static const float DATA_MAX = 3272; 

static const float EPSILON = 0.01;

// COLORS / barvicty
#define BLACK v3(0.f, 0.f, 0.f)
#define WHITE v3(1.f, 1.f, 1.f)
#define RED   v3(1.f, 0.f, 0.f)
#define GREEN v3(0.f, 1.f, 0.f)
#define BLUE  v3(0.f, 0.f, 1.f)
#define YELLOW v3(1.f, 1.f, 0.f)
#define CYAN  v3(0.f, 1.f, 1.f)
#define MAGENTA v3(1.f, 0.f, 1.f)

#endif