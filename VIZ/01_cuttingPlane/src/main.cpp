#define USE_ANTTWEAKBAR
#define NULL 0

#include "settings.h"
#include <stdarg.h>
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
GLuint		cmID;
v3			ctNormal(0.f, 0.f, 1.f);
float		ctZ = 0;
float		z = -2.0;
float  nDir[3] = {ctNormal.x, ctNormal.y, ctNormal.z};
double time = 0;
bool  g_bCubeRotation = false;             // Cube rotation enabled
float rotAngle = 0;


// FORWARD DECLARATIONS________________________________________________________
void initGUI(void);

//vykresleni textu ve 3D, pouzito pro skore
void strokeOutput(char *format,...) {
	va_list args;
	char buffer[200], *p;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	  
	glLineWidth(1.0);
	for (p = buffer; *p; p++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	}
}

void drawNumber(int number) {
	glPushAttrib(GL_LINE_BIT);
	  strokeOutput("%i", number);
	glPopAttrib();
}

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
			glVertex3f(-1.f, -1.f, 0.f);

			glTexCoord2f(ctTex.width, 0.0f);
			glVertex3f(1.f, -1.f, 0.f);

			glTexCoord2f(ctTex.width, ctTex.height);
			glVertex3f(1.f, 1.f, 0.f);

			glTexCoord2f(0.0f, ctTex.height);
			glVertex3f(-1.f, 1.f, 0.f);

	glEnd();
}

void drawColorMap()
{
	glBegin(GL_QUADS);
		
			glTexCoord2f(0.f, 0.5f);
			glVertex3f(-1.f, -1.1f, 0.f);

			glTexCoord2f(ctTex.cm.cm.width, 0.5f);
			glVertex3f(1.f, -1.1f, 0.f);

			glTexCoord2f(ctTex.cm.cm.width, 0.5f);
			glVertex3f(1.f, -1.05f, 0.f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(-1.f, -1.05f, 0.f);

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
   glTranslatef(0.0f, 0.0f, z);
   glRotatef(s_SceneRotation, 1.0f, 1.0f, 1.0f);
   
   // enable texture
   glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		// draw cutting plane 
		drawPlane();
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cmID);
		// draw color map
		drawColorMap();
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

   glDisable(GL_TEXTURE_RECTANGLE_ARB);
   
  

  glPushMatrix();
	glTranslatef(-1.08f, -1.1f, 0.0f);
	glScalef(0.0005f, 0.0005f, 0.0005f);
	drawNumber(0);
  glPopMatrix();

  glPushMatrix();
	glTranslatef(1.03f, -1.1f, 0.0f);
	glScalef(0.0005f, 0.0005f, 0.0005f);
	drawNumber(3272);
  glPopMatrix();

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
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F, ctTex.width, ctTex.height, 0, GL_RGBA, GL_FLOAT, ctTex.getData());

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	glGenTextures(1, &cmID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cmID);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, ctTex.cm.cm.width, ctTex.cm.cm.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ctTex.cm.cm.getData());

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

	glDisable(GL_TEXTURE_RECTANGLE_ARB);

}

void updateTexture(){
	ctNormal.x = nDir[0];
	ctNormal.y = nDir[1];
	ctNormal.z = nDir[2];
	ctNormal.normalize();
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		ctTex.makeView(ctNormal, ctZ);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F, ctTex.width, ctTex.height, 0, GL_RGBA, GL_FLOAT, ctTex.getData());
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void updateColorMap() {
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cmID);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, ctTex.cm.cm.width, ctTex.cm.cm.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ctTex.cm.cm.getData());
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void TW_CALL cbUpdateTexture(void *clientData)
{
	updateTexture();
}

void TW_CALL cbUpdateTexture1(void *clientData)
{
	ctTex.changeCm("colorMaps/cm_2c.png");
	updateColorMap();
	updateTexture();
}

void TW_CALL cbUpdateTexture2(void *clientData)
{
	ctTex.changeCm("colorMaps/cm_2.png");
	updateColorMap();
	updateTexture();
}

void TW_CALL cbUpdateTexture3(void *clientData)
{
	ctTex.changeCm("colorMaps/spectrum.png");
	updateColorMap();
	updateTexture();
}

void TW_CALL cbAxisX(void *clientData)
{
	rotAngle = 270;
	nDir[0] = 1;
	nDir[1] = 0;
	nDir[2] = 0;
	updateTexture();
}

void TW_CALL cbAxisY(void *clientData)
{
	rotAngle = 0;
	nDir[0] = 0;
	nDir[1] = 1;
	nDir[2] = 0;
	updateTexture();
}

void TW_CALL cbAxisZ(void *clientData)
{
	rotAngle = 0;
	nDir[0] = 0;
	nDir[1] = 0;
	nDir[2] = 1;
	updateTexture();
}

void TW_CALL cbSetZ(const void *value, void *clientData)
{ 
    ctZ = *(const float*)value;  // for instance
	ctTex.makeView(ctNormal, ctZ);
	updateTexture();
}
void TW_CALL cbGetZ(void *value, void *clientData)
{ 
    *(float *)value = ctZ;  // for instance

}

void TW_CALL cbSetR(const void *value, void *clientData)
{ 
    rotAngle = *(const float*)value;  // for instance
	ctTex.makeView(ctNormal, ctZ);
	updateTexture();
}

void TW_CALL cbGetR(void *value, void *clientData)
{ 
    *(float *)value = rotAngle;  // for instance

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
   TwDefine(" Controls position='10 10' size='200 350' valueswidth=100 ");

   //TwAddVarRW(controlBar, "rotate", TW_TYPE_BOOLCPP, &g_bCubeRotation, " group='Scene' key=r help='Cube rotation.' ");
   //TwAddVarRW(controlBar, "z", TW_TYPE_FLOAT, &z, " group='Scene' min=-100 max=0 step=0.1 ");

   TwAddVarCB(controlBar, "Rotation", TW_TYPE_FLOAT, cbSetR, cbGetR, NULL, " group='CT' min=-360 max=360 step=1 ");
   
   TwAddVarRW(controlBar, "Normal", TW_TYPE_DIR3F, &nDir, " group='CT' ");
   
   TwAddVarCB(controlBar, "Cut", TW_TYPE_FLOAT, cbSetZ, cbGetZ, NULL, " group='CT' min=-200 max=200 step=1 ");
   
   TwAddButton(controlBar, "UPDATE", cbUpdateTexture, NULL, " group='CT' label='Update CT view' "); 

   TwAddButton(controlBar, "PohledX", cbAxisX, NULL, " group='Axis' label='Axis X' "); 
   TwAddButton(controlBar, "PohledY", cbAxisY, NULL, " group='Axis' label='Axis Y' ");
   TwAddButton(controlBar, "PohledZ", cbAxisZ, NULL, " group='Axis' label='Axis Z' ");

   TwAddButton(controlBar, "UPDATE1", cbUpdateTexture1, NULL, " group='ColorMap' label='Black - white' "); 
   TwAddButton(controlBar, "UPDATE2", cbUpdateTexture2, NULL, " group='ColorMap' label='Red - white' "); 
   TwAddButton(controlBar, "UPDATE3", cbUpdateTexture3, NULL, " group='ColorMap' label='Spectrum' "); 

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
	ctTex.init(500, 500, 4, "colorMaps/spectrum.png", "ctdata/cthead-16bit%03i.png", 113, 1,1,2);
	// create initial snapshot
	ctTex.makeView(ctNormal, 0.f);

}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
   initApp();
   return common_main(g_WindowWidth, g_WindowHeight,
                      "VIZ Adam Kucera, Roman Polasek",
                      cbInitGL,              // init GL callback function
                      cbDisplay,             // display callback function
                      cbWindowSizeChanged,   // window resize callback function
                      cbKeyboardChanged,     // keyboard callback function
                      NULL,                  // mouse button callback function
                      NULL                   // mouse motion callback function
                      );
}
