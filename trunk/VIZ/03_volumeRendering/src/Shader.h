#ifndef _SHADER_H
#define _SHADER_H

#include "Vector3.h"
#include "Vector4.h"
#include <string>

using namespace std;

class Shader
{
public:
	Shader(		 v3 _ambientColor,
				 v3 _diffuseColor,
				 v3 _specularColor,
				 float  _ka,
				 float	_kd,
				 float  _ks,
				 float  _shininess);

	~Shader(void);

	void apply(  v3 *outputColor,				
				 v3 *inColor,
				 v3	*nnormal,
				 v3	*lightDir,
				 v3	*viewerDir
			   );
	const char * getDescription(){
		return "Phong shader";
	}

	// VARIABLES
	v3 ambientColor;
	v3 diffuseColor;
	v3 specularColor;
	float  ka;
	float  kd;
	float  ks;
	float  shininess;
};

#endif
