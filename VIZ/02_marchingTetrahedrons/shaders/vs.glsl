#version 120

varying vec3 normal;
varying vec3 vertex;

void main() {

	vertex= vec3(gl_ModelViewMatrix * gl_Vertex);

	normal = gl_NormalMatrix * normalize(gl_Normal);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}