#ifndef _SHADER_H
#define _SHADER_H

#include "Vector3.h"
#include "Vector4.h"

class Shader
{
public:
	Shader(void);
	~Shader(void);
	void apply(  v3 *outputColor,
				 v3 *ambientColor,
				 v3 *diffuseColor,
				 v3 *specularColor,
				 float  ka,
				 float	kd,
				 float  ks,
				 float  shininess,
				 v3		*normal,
				 v3		*lightDir,
				 v3		*viewerDir
			   );

};

#endif
