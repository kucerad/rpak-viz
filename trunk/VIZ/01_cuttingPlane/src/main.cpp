//-----------------------------------------------------------------------------
//  [PGR2] Geometry transport test case
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//		[SPACE]  ... change geometry transport mode
//		[r]      ... toggle cube rotation
//-----------------------------------------------------------------------------
#define USE_ANTTWEAKBAR
#define NULL 0


#include "../common/common.h"
#include "../common/models/cube.h"
#include "../common/png.h"
#include "../common/utils.h"
#include <assert.h>
#include "ctData.h"

// GLOBAL CONSTANTS____________________________________________________________
#define BUFFER_OFFSET(i) ((char*) NULL + (i))

// GLOBAL VARIABLES____________________________________________________________
GLint g_WindowWidth   = 800;              // Window width
GLint g_WindowHeight  = 600;              // Window height

// data of cts
CTdata ct;

bool  g_bCubeRotation = true;             // Cube rotation enabled

// FORWARD DECLARATIONS________________________________________________________
void initGUI(void);

//-----------------------------------------------------------------------------
// Name: immediateMode()
// Desc: 
//-----------------------------------------------------------------------------
void drawCube()
{
	glBegin(GL_QUADS);
		for (int iVertex = 0; iVertex < NUM_CUBE_VERTICES; iVertex++)
		{
			glColor3fv (&CUBE_COLOR_ARRAY[3*iVertex]);
			glVertex3fv(&CUBE_VERTEX_ARRAY[3*iVertex]);
		}
	glEnd();
}

//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{
   static GLfloat s_SceneRotation = 0.0f;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// Setup camera
   glLoadIdentity();
   glTranslatef(0.0f, 0.0f, -5.0f);
   glRotatef(s_SceneRotation, 1.0f, 1.0f, 1.0f);

   drawCube();

   s_SceneRotation += (g_bCubeRotation) ? 1.0f : 0.0f;
}


//-----------------------------------------------------------------------------
// Name: cbInitGL()
// Desc: 
//-----------------------------------------------------------------------------
void cbInitGL()
{
   // Init app GUI
   initGUI();

   // Setup OpenGL state variables
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}


//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: 
//-----------------------------------------------------------------------------
void initGUI()
{
#ifdef USE_ANTTWEAKBAR
   // Initialize AntTweakBar GUI
   if (!TwInit(TW_OPENGL, NULL))
   {
      assert(0);
   }

   TwWindowSize(g_WindowWidth, g_WindowHeight);
   TwBar *controlBar = TwNewBar("Ovládání");
   TwDefine(" Controls position='10 10' size='290 90' valueswidth=140 ");

   TwAddVarRW(controlBar, "rotate", TW_TYPE_BOOLCPP, &g_bCubeRotation,
              " group='Scene' key=r help='Cube rotation.' ");
#endif
}


//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbWindowSizeChanged(int width, int height)
{
   glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0f, GLfloat(width) / height, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);

   g_WindowWidth  = width;
   g_WindowHeight = height;
}


//-----------------------------------------------------------------------------
// Name: cbKeyboardChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbKeyboardChanged(int key, int action)
{
#ifndef USE_ANTTWEAKBAR
	if (key == GLFW_KEY_SPACE)
   {
      g_TransportMode = (g_TransportMode + 1) % NUM_TRANSPORT_MODES;
   }
#endif
}
void initApp(){
	// load CTs
	ct.loadFromFiles("ctdata/cthead-16bit%03i.png", 113, 1,1,2);
	float val = interpolate3Dlin(0,1,2,3,4,5,6,7,1,0,0);
	/*PNG pngFile;
	pngFile.load("ctdata/cthead-16bit015.png");
	for (int i=0; i<pngFile.width; i++){
		for (int j=0; j<pngFile.height; j++){
			if (pngFile.getPixelRaw(i, j)>32768){
				printf("POZOR\n");
			}
		}
	}*/
	//int pix = pngFile.getPixelRaw(130, 100);
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
   initApp();
   return common_main(g_WindowWidth, g_WindowHeight,
                      "[PGR2] Geometry transport test case",
                      cbInitGL,              // init GL callback function
                      cbDisplay,             // display callback function
                      cbWindowSizeChanged,   // window resize callback function
                      cbKeyboardChanged,     // keyboard callback function
                      NULL,                  // mouse button callback function
                      NULL                   // mouse motion callback function
                      );
}