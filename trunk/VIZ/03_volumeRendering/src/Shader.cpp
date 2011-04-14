#include "Shader.h"


Shader::Shader(		 v3 _ambientColor,
					 v3 _diffuseColor,
					 v3 _specularColor,
					 float  _ka,
					 float	_kd,
					 float  _ks,
					 float  _shininess):
	ambientColor(_ambientColor),
	diffuseColor(_diffuseColor),
	specularColor(_specularColor),
	ka(_ka), kd(_kd), ks(_ks), shininess(_shininess)
{
}


Shader::~Shader(void)
{
}

void Shader::apply( v3  *outputColor,				
					v3  *inColor,
					v3	*nnormal,
					v3	*lightDir,
					v3	*viewerDir
			   )
{
	v3 normal = nnormal->getNormalized();
	float NdotL = lightDir->dot(normal);
	v3 R = normal*(NdotL*2.0) - (*lightDir);
	float RdotV = R.dot(*viewerDir);
	
	*outputColor = (ambientColor) * ka + (*inColor)*(kd*NdotL) + (specularColor)*ks*pow(RdotV, shininess);
}