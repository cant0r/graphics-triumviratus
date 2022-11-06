#version 430

// aPos		= attributum position
layout (location = 0) in vec3 aPos;

out vec3 myColor;
out vec3 myPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 invTMatrix;

void main(void)
{
	myColor = vec3(1.0,1.0,1.0);
	myPos = vec3(view * model * vec4(aPos, 1.0));
	gl_Position = projection * vec4(myPos, 1.0);
}