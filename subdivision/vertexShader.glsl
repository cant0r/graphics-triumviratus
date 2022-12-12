#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 myColor;
out vec3 myPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 invTMatrix;

void main(void)
{
	myColor = aColor;
	myPos = vec3(view * model * vec4(aPos, 1.0));
	gl_Position = projection * vec4(myPos, 1.0);
}