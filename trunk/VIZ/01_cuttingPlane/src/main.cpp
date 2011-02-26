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


#include "common.h"
#include "models/cube.h"
#include "png.h"
#include "utils.h"
#include <assert.h>
#include "ctTexture.h"


// GLOBAL CONSTANTS____________________________________________________________
#define BUFFER_OFFSET(i) ((char*) NULL + (i))

// GLOBAL VARIABLES____________________________________________________________
GLint g_WindowWidth   = 800;              // Window width
GLint g_WindowHeight  = 600;              // Window height

CTtexture	ctTex;
GLuint		textureID;
v3			ctNormal(0.f, 0.f, 1.f);
float		ctZ = 0;

float  nDir[3] = {ctNormal.x, ctNormal.y, ctNormal.z};

bool  g_bCubeRotation = false;             // Cube rotation enabled

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
void drawPlane()
{
	glBegin(GL_QUADS);
		
			glTexCoord2f(0.f, 0.0f);
			//glColor3f (1.f, 0.f, 0.f);
			glVertex3f(-1.f, -1.f, 0.f);

			glTexCoord2f(ctTex.width, 0.0f);
			//glColor3f (0.f, 1.f, 0.f);
			glVertex3f(1.f, -1.f, 0.f);

			glTexCoord2f(ctTex.width, ctTex.height);
			//glColor3f (0.f, 0.f, 1.f);
			glVertex3f(1.f, 1.f, 0.f);

			glTexCoord2f(0.0f, ctTex.height);
			//glColor3f (1.f, 0.f, 1.f);
			glVertex3f(-1.f, 1.f, 0.f);

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
  glDisable(GL_LIGHTING);
	// Setup camera
   glLoadIdentity();
   glTranslatef(0.0f, 0.0f, -5.0f);
   glRotatef(s_SceneRotation, 1.0f, 1.0f, 1.0f);

   //drawCube();
   //drawPlane();
   
   // enable texture
   glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		// draw cutting plane 
		drawPlane();
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
   glDisable(GL_TEXTURE_RECTANGLE_ARB);
  
  glEnable(GL_LIGHTING);
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

	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	//glClientActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F, ctTex.width, ctTex.height, 0, GL_RGBA, GL_FLOAT, ctTex.getData());

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}
void TW_CALL cbUpdateTexture(void *clientData)
{
	ctNormal.x = nDir[0];
	ctNormal.y = nDir[1];
	ctNormal.z = nDir[2];
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		ctTex.makeView(ctNormal, ctZ);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F, ctTex.width, ctTex.height, 0, GL_RGBA, GL_FLOAT, ctTex.getData());
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
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
   TwDefine(" Controls position='10 10' size='290 90' valueswidth=140 ");

   TwAddVarRW(controlBar, "rotate", TW_TYPE_BOOLCPP, &g_bCubeRotation,
              " group='Scene' key=r help='Cube rotation.' ");

   TwAddVarRW(controlBar, "Normal", TW_TYPE_DIR3F, &nDir, "");
   TwAddVarRW(controlBar, "z", TW_TYPE_FLOAT, &ctZ, " min=-100 max=100 step=0.1 ");
   TwAddButton(controlBar, "UPDATE", cbUpdateTexture, NULL, " label='Update CT view' "); 

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
	ctTex.init(500, 500, 4, "colorMaps/cm_2c.png", "ctdata/cthead-16bit%03i.png", 113, 1,1,2);
	// create initial snapshot
	ctTex.makeView(ctNormal, 0.f);

	
	// load CTs
	//ct.loadFromFiles("ctdata/cthead-16bit%03i.png", 113, 1,1,2);
	//ColorMap cm;
	//cm.loadFromFile("colorMaps/cm_Black2White200.png");
	//cm.loadFromFile("colorMaps/cm_2.png");
	//v4 val = cm.mapValueToColor(0.5f);
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