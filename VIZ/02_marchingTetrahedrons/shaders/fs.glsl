#version 120

varying vec3 normal;
varying vec3 vertex;

void main() {
	vec3 N = normalize(normal);
	vec3 L = normalize(gl_LightSource[0].position.xyz- vertex);
	vec3 E = normalize(-vertex);
	vec3 R = normalize(-reflect(L,N));

	float NdotL = max(dot(normalize(N),normalize(L)), 0.0);
	float RdotE = max(dot(R,E),0.0);

	vec4 Ia = gl_FrontLightProduct[0].ambient;
	vec4 Id = gl_FrontLightProduct[0].diffuse * NdotL;
	vec4 Is = gl_FrontLightProduct[0].specular * pow(RdotE, gl_FrontMaterial.shininess);

	gl_FragColor = gl_FrontLightModelProduct.sceneColor + Ia + Id +Is;

	
}