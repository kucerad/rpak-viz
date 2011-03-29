//-----------------------------------------------------------------------------
//  [PGR2] Simple GLSL Example
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//    [mouse-left-button]  ... scene rotation
//    [t], [T]             ... move scene forward/backward
//    [r]                  ... toggle scene rotation
//    [v]                  ... toggle vertex shader
//    [g]                  ... toggle geometry shader
//    [f]                  ... toggle fragment shader
//    [s]                  ... toggle programmable pipeline (shaders)
//    [b]                  ... build shader programs
//    [w]                  ... toggle wire mode
//    [c]                  ... toggle face culling
//    [space]              ... change model type
//-----------------------------------------------------------------------------
#define USE_ANTTWEAKBAR
#define NULL 0

#include "settings.h"
#include <stdarg.h>
#include "common.h"
#include "png.h"
#include "utils.h"
#include <assert.h>
#include "ctTexture.h"
#include "ctData.h"
//#include "./GL/glut.h"

// GLOBAL CONSTANTS____________________________________________________________
const char* VS_FILE_NAME     = "shaders/vs.glsl";  // Vertex shader source file
const char* GS_FILE_NAME     = "shaders/gs.glsl";  // Geometry shader source file
const char* FS_FILE_NAME     = "shaders/fs.glsl";  // Fragment shader source file

// GLOBAL VARIABLES____________________________________________________________
GLint    g_WindowWidth       = 800;    // Window width
GLint    g_WindowHeight      = 600;    // Window height

GLfloat  g_SceneRot[]        = { 0.0f, 0.0f, 0.0f, 1.0f }; // Scene orientation
GLfloat  g_SceneTraX         = 0.0f;   // Scene translation along x-axis
GLfloat  g_SceneTraY         = 0.0f;   // Scene translation along y-axis
GLfloat  g_SceneTraZ         = 5.0f;   // Scene translation along z-axis
bool     g_SceneRotEnabled   = false;  // Scene auto-rotation enabled/disabled
bool     g_WireMode          = false;  // Wire mode enabled/disabled
bool     g_FaceCulling       = false;  // Face culling enabled/disabled

bool     g_UseShaders        = true;  // Programmable pipeline on/off
bool     g_UseVertexShader   = true;  // Use vertex shader
bool     g_UseGeometryShader = false;  // Use geometry shader
bool     g_UseFragmentShader = true;  // Use fragment shader
CTdata	 dataCT;

enum EGeometry                         // Geometry type enum
{	
   ELEPHANT_GEOMETRY = 0, 
   CUBE_GEOMETRY,
   NUM_GEOMETRY_TYPES
};
int      g_GeometryType       = ELEPHANT_GEOMETRY; // Geometry type


// GLSL variables
GLuint   g_ProgramId       = 0;      // Shader program id

// FORWARD DECLARATIONS________________________________________________________
#ifdef USE_ANTTWEAKBAR
   void TW_CALL cbSetShaderStatus(const void*, void*);
   void TW_CALL cbGetShaderStatus(void*, void*);
#endif
void TW_CALL cbCompileShaderProgram(void *clientData);
void initGUI();


//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{
   static GLfloat scene_rot = 0.0f;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
   (g_FaceCulling) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	// Setup camera
   glLoadIdentity();
   glTranslatef(g_SceneTraX, g_SceneTraY, -g_SceneTraZ);
   pgr2AddQuaternionRotationToTransformation(g_SceneRot);
   glRotatef(scene_rot, 0.0f, 1.0f, 0.0f);

   // Turn on programmable pipeline
   if (g_UseShaders)
   {
		   glUseProgram(g_ProgramId);    // Active shader program
   }

   // Draw model
   //glColor3f(1.0, 0.0, 0.0);
   dataCT.draw3dIsosurface();

   // Turn off programmable pipeline
   glUseProgram(NULL);

   if (g_SceneRotEnabled)
   {
      scene_rot++;
   }
}

void initApp()
{
	dataCT.loadFromFiles("ctdata/cthead-16bit%03i.png", 113, 1,1,1);
	dataCT.create3dIsosurface(500, 1, 1, 1);
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
   glClearColor(0.4f, 0.4f, 0.7f, 0);
   glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   (g_FaceCulling) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

   GLfloat light_amb[4] = {0.4f, 0.4f, 0.4f, 1.0f};
   GLfloat light_dif[4] = {0.0f, 0.0f, 0.0f, 1.0f};
   GLfloat light_spe[4] = {0.5f, 0.5f, 0.5f, 1.0f};
   GLfloat light_pos[4] = {0.0f, 3.0f, 3.0f, 1.0f};
   GLfloat light_dir[4] = {0.0f,-1.5f,-3.0f, 1.0f};
	
   GLfloat material_amd[4] = {1.0f, 0.1f, 0.1f, 1.0f};
   GLfloat material_spe[4] = {0.8f, 0.8f, 0.8f, 1.0f};

   glMaterialfv(GL_FRONT_AND_BACK,  GL_AMBIENT, material_amd);
   glMaterialfv(GL_FRONT_AND_BACK,  GL_DIFFUSE, material_amd);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_spe);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

   glEnable(GL_LIGHT1);
   glLightfv(GL_LIGHT1, GL_POSITION      , light_pos);
   glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_dir);
   glLightfv(GL_LIGHT1, GL_AMBIENT       , light_amb);
   glLightfv(GL_LIGHT1, GL_DIFFUSE       , light_dif);
   glLightfv(GL_LIGHT1, GL_SPECULAR	     , light_spe);
   glLightf (GL_LIGHT1, GL_SPOT_CUTOFF	  , 24);
   glLightf (GL_LIGHT1, GL_SPOT_EXPONENT , 128);

   cbCompileShaderProgram(NULL);
}


//-----------------------------------------------------------------------------
// Name: cbCompileShaderProgram()
// Desc: 
//-----------------------------------------------------------------------------
void TW_CALL cbCompileShaderProgram(void *clientData)
{
   // Delete shader program if exists
   if (g_ProgramId)
   {
      glDeleteProgram(g_ProgramId);
   }

   // Create shader program object
   g_ProgramId = glCreateProgram();

   if (g_UseVertexShader)
   {
      // Create shader objects for vertex shader
      GLuint id = pgr2CreateShaderFromFile(GL_VERTEX_SHADER, VS_FILE_NAME);
      glAttachShader(g_ProgramId, id);
      glDeleteShader(id);
   }
   if (g_UseGeometryShader)
   {
      // Create shader objects for geometry shader
      GLuint id = pgr2CreateShaderFromFile(GL_GEOMETRY_SHADER, GS_FILE_NAME);
      glAttachShader(g_ProgramId, id);
      glDeleteShader(id);
	   glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_VERTICES_OUT_EXT, 6);
	   glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_INPUT_TYPE_EXT, 
                             GL_TRIANGLES);
	   glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_OUTPUT_TYPE_EXT, 
                             GL_TRIANGLE_STRIP);
   }
   if (g_UseFragmentShader)
   {
      // Create shader objects for fragment shader
      GLuint id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FS_FILE_NAME);
      glAttachShader(g_ProgramId, id);
      glDeleteShader(id);
   }

   // Link shader program
   glLinkProgram(g_ProgramId);
   if (!pgr2CheckProgramLinkStatus(g_ProgramId))
   {
      pgr2CheckProgramInfoLog(g_ProgramId);
      printf("Shader program creation failed.\n\n");
      glDeleteProgram(g_ProgramId);
      g_ProgramId  = 0;
      g_UseShaders = false;
      return;
   }
   else
   {
      printf("Shader program compiled successfully.\n\n");
   }
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
   TwDefine(" Controls position='10 10' size='200 320' refresh=0.1 ");
   /*
   TwAddVarCB(controlBar, "use_shaders", TW_TYPE_BOOLCPP, cbSetShaderStatus,
      cbGetShaderStatus, NULL, " label='shaders' key=s help='Turn \
      programmable pipeline on/off.' ");

   // Shader panel setup
   TwAddVarRW(controlBar, "vs", TW_TYPE_BOOLCPP, &g_UseVertexShader,
    " group='Shaders' label='vertex' key=v help='Toggle vertex shader.' ");
   TwAddVarRW(controlBar, "gs", TW_TYPE_BOOLCPP, &g_UseGeometryShader,
    " group='Shaders' label='geometry' key=g help='Toggle geometry shader.' ");
   TwAddVarRW(controlBar, "fs", TW_TYPE_BOOLCPP, &g_UseFragmentShader,
    " group='Shaders' label='fragment' key=f help='Toggle fragment shader.' ");
   TwAddButton(controlBar, "build", cbCompileShaderProgram, NULL, 
    " group='Shaders' label='build' key=b help='Build shader program.' ");
// TwDefine( " Controls/Shaders readonly=true "); 

*/
   // Render panel setup
   TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &g_WireMode,
    " group='Render' label='wire mode' key=w help='Toggle wire mode.' ");
   TwAddVarRW(controlBar, "face_culling", TW_TYPE_BOOLCPP, &g_FaceCulling,
      " group=Render label='face culling' key=c help='Toggle face culling.' ");

   // Scene panel setup
   TwEnumVal geometry_type[] = 
   { 
      { ELEPHANT_GEOMETRY    , "Elephant"},
      { CUBE_GEOMETRY        , "Cube"    },
   };
   TwType geom_type = TwDefineEnum("Model", geometry_type, 
                                        NUM_GEOMETRY_TYPES);
   TwAddVarRW(controlBar, "model", geom_type, &g_GeometryType, 
              " group='Scene' keyIncr=Space help='Change model.' ");
   TwAddVarRW(controlBar, "auto-rotation", TW_TYPE_BOOLCPP, &g_SceneRotEnabled,
    " group='Scene' label='rotation' key=r help='Toggle scene rotation.' ");
   TwAddVarRW(controlBar, "TranslateX", TW_TYPE_FLOAT, &g_SceneTraX, 
    " group='Scene' label='translate X' min=-500 max=500 step=0.1 \
    keyIncr=x keyDecr=X help='Scene translation X.' ");
   TwAddVarRW(controlBar, "TranslateY", TW_TYPE_FLOAT, &g_SceneTraY, 
    " group='Scene' label='translate Y' min=-500 max=500 step=0.1 \
    keyIncr=y keyDecr=Y help='Scene translation Y.' ");
   TwAddVarRW(controlBar, "TranslateZ", TW_TYPE_FLOAT, &g_SceneTraZ, 
    " group='Scene' label='translate Z' min=1 max=1000 step=0.1 \
    keyIncr=z keyDecr=Z help='Scene translation Z.' ");
   TwAddVarRW(controlBar, "SceneRotation", TW_TYPE_QUAT4F, &g_SceneRot, 
    " group='Scene' label='rotation' open help='Toggle scene orientation.' ");
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
   gluPerspective(55.0f, GLfloat(width)/height, 0.1f, 1000.0f);
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
	   case GLFW_KEY_SPACE:
         g_GeometryType = (g_GeometryType + 1) % NUM_GEOMETRY_TYPES;
      break;
	  case 'x' : g_SceneTraX        += 0.1f;                               break;
	  case 'X' : g_SceneTraX        -= 0.1f;							   break;
	  case 'y' : g_SceneTraY        += 0.1f;                               break;
	  case 'Y' : g_SceneTraY        -= 0.1f;							   break;
      case 'z' : g_SceneTraZ        += 0.1f;                               break;
      case 'Z' : g_SceneTraZ        -= (g_SceneTraZ > 0.5) ? 0.1f : 0.0f;  break;
      case 'r' : g_SceneRotEnabled   = !g_SceneRotEnabled;                 break;
      case 'v' : g_UseVertexShader   = !g_UseVertexShader;                 break;
      case 'g' : g_UseFragmentShader = !g_UseFragmentShader;               break;
      case 'f' : g_UseGeometryShader = !g_UseGeometryShader;               break;
      case 'w' : g_WireMode          = !g_WireMode;                        break;
      case 'c' : g_FaceCulling       = !g_FaceCulling;                     break;
      case 's' : 
         g_UseShaders = !g_UseShaders;
      case 'b' : 
         cbCompileShaderProgram(NULL);
         return;
      break;
	}

   printf("[s] g_UseShaders         %s\n", g_UseShaders ? "true" : "false");
   printf("[v] g_UseVertexShader    %s\n", g_UseVertexShader ? "true" : "false");
   printf("[f] g_UseFragmentShader  %s\n", g_UseFragmentShader ? "true" : "false");
   printf("[g] g_UseGeometryShader  %s\n", g_UseGeometryShader ? "true" : "false");
   printf("[w] g_WireMode           %s\n", g_WireMode ? "true" : "false");
   printf("[c] g_FaceCulling        %s\n\n", g_FaceCulling ? "true" : "false");
}


#ifdef USE_ANTTWEAKBAR
//-----------------------------------------------------------------------------
// Name: cbSetShaderStatus()
// Desc: 
//-----------------------------------------------------------------------------
void TW_CALL cbSetShaderStatus(const void *value, void *clientData)
{
   g_UseShaders = *(bool*)(value);
   // Try to compile shader program
   if (g_UseShaders)
   {
      cbCompileShaderProgram(NULL);
   }
// TwDefine((g_UseShaders) ? " Controls/Shaders readonly=false " : 
//                           " Controls/Shaders readonly=true "); 
}


//-----------------------------------------------------------------------------
// Name: cbGetShaderStatus()
// Desc: 
//-----------------------------------------------------------------------------
void TW_CALL cbGetShaderStatus(void *value, void *clientData)
{
    *(bool*)(value) = g_UseShaders;
} 
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
