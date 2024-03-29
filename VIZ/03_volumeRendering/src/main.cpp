#define USE_ANTTWEAKBAR
#define NULL 0
#define TESTMODE 0


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


GLint    mouseX=0, mouseY = 0;
Camera   *pCamera;
v3			camDir(1.f, 0.f, 0.f);
v3			camUp(0.f, 0.f, 1.f);
float		camDistance = 200.0;
float  nDir[3] = {camDir.x, camDir.y, camDir.z};

ColorMap* pColorMap = new ColorMap();
CTdata* pDataCT = new CTdata();

GLuint		textureID;

#define TRANSFER_F_FILENAME "colorMaps/tf04.png"
//#define TRANSFER_F_FILENAME "colorMaps/spectrumAlpha2.png"

float texWidth = float(g_WindowWidth)/float(g_WindowHeight);
float texHeight = float(g_WindowHeight)/float(g_WindowHeight);


// FORWARD DECLARATIONS________________________________________________________
void initGUI();

void drawPlane()
{
	glBegin(GL_QUADS);
		
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-texWidth, -texHeight, 0.0f);

			glTexCoord2f(g_WindowWidth, 0.0f);
			glVertex3f(texWidth, -texHeight, 0.0f);

			glTexCoord2f(g_WindowWidth, g_WindowHeight);
			glVertex3f(texWidth, texHeight, 0.0f);

			glTexCoord2f(0.0f, g_WindowHeight);
			glVertex3f(-texWidth, texHeight, 0.0f);

	glEnd();
}
//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // draw pixels
   //glDrawPixels(g_WindowWidth,g_WindowHeight, GL_RGB, GL_FLOAT, pCamera->imageData);
 
   glDisable(GL_LIGHTING);

   glPushMatrix();
   glTranslated(0.0, 0.0, -2.0);
   glColor3f(1.0, 1.0, 0.0);
   //glutSolidSphere(1.1, 10, 10);
   glEnable(GL_TEXTURE_RECTANGLE_ARB);
   glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	drawPlane();
   glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
   glPopMatrix();
}


void updateTexture() {
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, g_WindowWidth, g_WindowHeight, 0, GL_RGB, GL_FLOAT, pCamera->imageData);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void updateView() {
	// TODO odladit
	v3 dir(nDir[0], nDir[1], nDir[2]);
	dir.normalize();
	float angle = dir.angleTo(camDir);
	v3 axis = dir.cross(camDir);


	//float angle = camDir.angleTo(Vector3(0.0, 0.0, 1.0));
	//v3 axis = camDir.cross(Vector3(0.0, 0.0, 1.0));
	
	pCamera->direction = dir;

	// recalculate up-vector & right-vector
	if (axis.length()>= 0.01){
		pCamera->up = camUp.getRotated(angle, axis);
	} else {
		pCamera->up = camUp;
	}
	pCamera->right = pCamera->up.cross(pCamera->direction);

	pCamera->position = pDataCT->center + pCamera->direction * (-camDistance);
	pCamera->printOut();
	// take a picture 
	pCamera->snapShot(pDataCT, 3);

	updateTexture();

}

void initApp()
{
	int textLenght = 0;

	textLenght = printf("Loading transfer function from file: '%s'.", TRANSFER_F_FILENAME);

	pColorMap->loadFromFile(TRANSFER_F_FILENAME);
	BACKSPACE(textLenght);
	printf("Transfer function loaded successfully.\n");


//	v4 color = pColorMap->mapValueToColor(0.152811f);
//	printf("COLOR[ %f %f %f %f ]\n", color.r, color.g,color.b,color.a);
//	system("PAUSE");

	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);


	textLenght = printf("Initializing shader.");
	Shader* pShader = new Shader(BLACK, BLACK, WHITE, 1.0f, 1.0f, 0.1f, 1.0f); 
	BACKSPACE(textLenght);
	printf("%s initialized.\n", pShader->getDescription());

	textLenght = printf("Loading data. ");
	pDataCT->setCm(pColorMap);
	pDataCT->setSh(pShader);

#if TESTMODE
	pDataCT->loadSphere(10,10,10, 0.f, 10.f);
#else
	pDataCT->loadFromFiles("ctdata/cthead-16bit%03i.png", 113, 1,1,2);
#endif
	BACKSPACE(textLenght);
	printf("CT data loaded successfully.\n");	
	
	// Cim mensi cislo SCALE, tim vetsi obrazek [priblizeni]
	// pro obrazek v puvodni velikosti... scale = 1;, 2x vetsi... scale = 0.5
#if TESTMODE
	float scale = 0.05f;
#else
	float scale = 0.5;
#endif	
	pCamera = new Camera(v3(0.f, 0.f, 200.f), v3(0.f, 0.f, 1.f), v3(1.f, 0.f, 0.f),g_WindowWidth,g_WindowHeight, scale);
	//pCamera = new Camera(v3(0.f, 0.f, 50.f), v3(0.f, 1.f, 0.f), v3(0.f, 0.f, -1.f),g_WindowWidth,g_WindowHeight, scale);
	
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
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void TW_CALL cbUpdate(void *clientData)
{
	updateView();
	updateTexture();
}

void TW_CALL cbFront(void *clientData)
{
	//settings
	nDir[0] = 0.0f;
	nDir[1] = 1.0f;
	nDir[2] = 0.0f;
	updateView();
}

void TW_CALL cbBack(void *clientData)
{
	//settings
	nDir[0] = 0.0f;
	nDir[1] = -1.0f;
	nDir[2] = 0.0f;
	updateView();
}

void TW_CALL cbTop(void *clientData)
{
	//settings
	nDir[0] = 0.0f;
	nDir[1] = 0.0f;
	nDir[2] = 1.0f;
	updateView();
}

void TW_CALL cbBottom(void *clientData)
{
	//settings
	nDir[0] = 0.0f;
	nDir[1] = 0.0f;
	nDir[2] = -1.0f;
	updateView();
}

void TW_CALL cbLeft(void *clientData)
{
	//settings
	nDir[0] = 1.0f;
	nDir[1] = 0.0f;
	nDir[2] = 0.0f;
	updateView();
}

void TW_CALL cbRight(void *clientData)
{
	//settings
	nDir[0] = -1.0f;
	nDir[1] = 0.0f;
	nDir[2] = 0.0f;
	updateView();
}

void TW_CALL cbUpdateTexture1(void *clientData)
{
	pColorMap->loadFromFile("colorMaps/tf01.png");
	pDataCT->setCm(pColorMap);
	pCamera->snapShot(pDataCT, 3);
	updateTexture();
}

void TW_CALL cbUpdateTexture2(void *clientData)
{
	pColorMap->loadFromFile("colorMaps/tf02.png");
	pDataCT->setCm(pColorMap);
	pCamera->snapShot(pDataCT, 3);
	updateTexture();
}

void TW_CALL cbUpdateTexture3(void *clientData)
{
	pColorMap->loadFromFile("colorMaps/tf03.png");
	pDataCT->setCm(pColorMap);
	pCamera->snapShot(pDataCT, 3);
	updateTexture();
}

void TW_CALL cbUpdateTexture4(void *clientData)
{
	pColorMap->loadFromFile("colorMaps/tf04.png");
	pDataCT->setCm(pColorMap);
	pCamera->snapShot(pDataCT, 3);
	updateTexture();
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
   TwDefine(" Controls position='3 3' size='220 370' refresh=0.1 ");
  // TwAddVarRO(controlBar, "progress",  TW_TYPE_FLOAT, &(pCamera->progress), " group='Refresh' label='Progress' "); 
   TwAddVarRW(controlBar, "direction", TW_TYPE_DIR3F, &nDir, " group='Refresh' label='Camera direction'");
   //TwAddVarRO(controlBar, "mouseX", TW_TYPE_INT32, &mouseX, " group='Refresh' label='MouseX'");
   //TwAddVarRO(controlBar, "mouseY", TW_TYPE_INT32, &mouseY, " group='Refresh' label='MouseY'");
   TwAddButton(controlBar, "Update_view", cbUpdate, NULL, " group='Refresh' label='Update view' "); 


   TwAddButton(controlBar, "Front", cbFront, NULL, " group='Fixed look' label='Front' "); 
   TwAddButton(controlBar, "Back", cbBack, NULL, " group='Fixed look' label='Back' ");
   TwAddButton(controlBar, "Top", cbTop, NULL, " group='Fixed look' label='Top' ");
   TwAddButton(controlBar, "Bottom", cbBottom, NULL, " group='Fixed look' label='Bottom' "); 
   TwAddButton(controlBar, "Left", cbLeft, NULL, " group='Fixed look' label='Left' ");
   TwAddButton(controlBar, "Right", cbRight, NULL, " group='Fixed look' label='Right' ");

   TwAddButton(controlBar, "Transfer function1", cbUpdateTexture1, NULL, " group='Transfer function' label='100% bones' "); 
   TwAddButton(controlBar, "Transfer function2", cbUpdateTexture2, NULL, " group='Transfer function' label='50% bones' "); 
   TwAddButton(controlBar, "Transfer function3", cbUpdateTexture3, NULL, " group='Transfer function' label='100% bones, 50% skin' "); 
   TwAddButton(controlBar, "Transfer function4", cbUpdateTexture4, NULL, " group='Transfer function' label='100% bones, 50% tissue' "); 

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
   switch (key)
   {
      case 'q' : 
         exit(0);
      break;
	}
}
void cbMousePositionChanged(int x, int y)
{
   mouseX = x;
   mouseY = y;
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
                      cbMousePositionChanged                   // mouse motion callback function
#else
                      cbMouseButtonChanged,  // mouse button callback function
                      cbMousePositionChanged // mouse motion callback function
#endif
                      );
}
