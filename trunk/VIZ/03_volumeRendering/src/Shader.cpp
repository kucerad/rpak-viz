#include "Shader.h"


Shader::Shader(void)
{
}


Shader::~Shader(void)
{
}

void Shader::apply(  v3 *outputColor,
					 v3 *ambientColor,
					 v3 *diffuseColor,
					 v3 *specularColor,
					 float  ka,
					 float	kd,
					 float  ks,
					 float  shininess,
					 v3		*normal,
					 v3		*lightDir,
					 v3		*viewerDir)
{
	float NdotL = lightDir->dot(*normal);
	v3 R = (*normal)*(NdotL*2.0) - (*lightDir);
	float RdotV = R.dot(*viewerDir);
	
	Color c = (*ambientColor) * ka + (*diffuseColor)*(kd*NdotL) + (*specularColor)*ks*pow(RdotV, shininess);
	return c;

}