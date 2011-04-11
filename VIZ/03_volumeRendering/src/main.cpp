#define USE_ANTTWEAKBAR
#define NULL 0
#include "settings.h"
#include <stdarg.h>
#include "common.h"
#include "png.h"
#include "utils.h"
#include <assert.h>
#include "ctData.h"
#include "Camera.h"
#include "BBox.h"
#include "Ray.h"
#include "colorMap.h"

// GLOBAL VARIABLES____________________________________________________________
GLint    g_WindowWidth       = 800;    // Window width
GLint    g_WindowHeight      = 600;    // Window height

CTdata	 *pDataCT;
Camera   *pCamera;
v3			camDir(0.f, 0.f, 1.f);
float		camDistance = 100.0;
float  nDir[3] = {camDir.x, camDir.y, camDir.z};

#define TRANSFER_F_FILENAME "colorMaps/spectrum.png"

// COLORS / barvicty
#define BLACK v3(0.f, 0.f, 0.f)
#define WHITE v3(1.f, 1.f, 1.f)
#define RED   v3(1.f, 0.f, 0.f)
#define GREEN v3(0.f, 1.f, 0.f)
#define BLUE  v3(0.f, 0.f, 1.f)
#define YELLOW v3(1.f, 1.f, 0.f)
#define CYAN  v3(0.f, 1.f, 1.f)
#define MAGENTA v3(1.f, 0.f, 1.f)

// FORWARD DECLARATIONS________________________________________________________
void initGUI();


//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{
   static GLfloat scene_rot = 0.0f;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // draw pixels
   glDrawPixels(g_WindowWidth,g_WindowHeight ,GL_RGB,GL_FLOAT,pCamera->imageData);
   
   glutSwapBuffers();
}
void updateView(){
	// TODO odladit
	camDir.x = nDir[0]; 
	camDir.y = nDir[1]; 
	camDir.z = nDir[2];
	camDir.normalize();
	float angle = camDir.angleTo(pCamera->direction);
	v3 axis = camDir.cross(pCamera->direction);
	

	pCamera->direction = camDir;

	// recalculate up-vector & right-vector
	if (axis.length()>= 0.01){
		pCamera->up.rotate(angle, axis);
	}
	pCamera->right = pCamera->up.cross(camDir);

	pCamera->position = pDataCT->center + camDir * (-camDistance);

	// take a picture 
	pCamera->snapShot(pDataCT, 3);
}
void initApp()
{
	int textLenght = 0;
	textLenght = printf("Loading transfer function from file: '%s'.", TRANSFER_F_FILENAME);
	ColorMap* pColorMap = new ColorMap();
	pColorMap->loadFromFile(TRANSFER_F_FILENAME);
	BACKSPACE(textLenght);
	printf("Transfer function loaded successfully.\n");
	textLenght = printf("Initializing shader.");
	Shader* pShader = new Shader(BLACK, BLACK, WHITE, 0.1f, 1.0f, 1.0f, 2.0f); 
	BACKSPACE(textLenght);
	printf("%s initialized.\n", pShader->getDescription());
	textLenght = printf("Loading data. ");
	pDataCT = new CTdata(pColorMap, pShader);
	pDataCT->loadFromFiles("ctdata/cthead-16bit%03i.png", 113, 1,1,2);
	BACKSPACE(textLenght);
	printf("CT data loaded successfully.\n");	
	pCamera = new Camera(v3(0.f, 0.f, 100.f), v3(0.f, 1.f, 0.f), v3(0.f, 0.f, -1.f),g_WindowWidth,g_WindowHeight);
	
	updateView();
}
//-----------------------------------------------------------------------------
// Name: cbInitGL()
// Desc: 
//-----------------------------------------------------------------------------
void cbInitGL()
{
   // Init app GUI
   initGUI();

   initApp();

	// Set OpenGL state variables
   glClearColor(0.0f, 0.0f, 0.0f, 1);
   glColor4f(.6f, .6f, .6f, 1.0f);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}
void TW_CALL cbUpdate(void *clientData)
{
	updateView();
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
   TwBar *controlBar = TwNewBar("Controls");
   TwDefine(" Controls position='10 10' size='200 370' refresh=0.1 ");
   TwAddVarRO(controlBar, "progress",  TW_TYPE_FLOAT, &(pCamera->progress), " group='Refresh' label='Progress' "); 
   TwAddVarRW(controlBar, "direction", TW_TYPE_DIR3F, &nDir, " group='Refresh' label='Camera direction'");
   

   TwAddButton(controlBar, "Update_view", cbUpdate, NULL, " group='Refresh' label='Update view' "); 
#endif
}


//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbWindowSizeChanged(int width, int height)
{
   glViewport(0, 0, width, height);
   
   glLoadIdentity();
   g_WindowWidth  = width;
   g_WindowHeight = height;
}


//-----------------------------------------------------------------------------
// Name: cbKeyboardChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbKeyboardChanged(int key, int action)
{
   switch (key)
   {
      case 'q' : 
         exit(0);
      break;
	}
}


#ifdef USE_ANTTWEAKBAR

#else
bool    g_MouseRotationEnabled   = false;

//-----------------------------------------------------------------------------
// Name: cbMouseButtonChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL cbMouseButtonChanged(int button, int action)
{
   g_MouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && 
                             (action == GLFW_PRESS));
}


//-----------------------------------------------------------------------------
// Name: cbMousePositionChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbMousePositionChanged(int x, int y)
{
   static int s_LastMousePoxX = x;
   static int s_LastMousePoxY = y;

   if (g_MouseRotationEnabled)
   {
      g_SceneRot[1] +=  0.9f*(x - s_LastMousePoxX);
      g_SceneRot[2] +=  0.9f*(y - s_LastMousePoxY);
      s_LastMousePoxX = x;
      s_LastMousePoxY = y;
   }
}
#endif


//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
   return common_main(g_WindowWidth, g_WindowHeight,
                      "VIZ Adam Kucera, Roman Polasek",
                      cbInitGL,              // init GL callback function
                      cbDisplay,             // display callback function
                      cbWindowSizeChanged,   // window resize callback function
                      cbKeyboardChanged,     // keyboard callback function
#ifdef USE_ANTTWEAKBAR
                      NULL,                  // mouse button callback function
                      NULL                   // mouse motion callback function
#else
                      cbMouseButtonChanged,  // mouse button callback function
                      cbMousePositionChanged // mouse motion callback function
#endif
                      );
}
