#version 430

// aPos		= attributum position
layout (location = 0) in vec3 aPos;

out vec3 myColor;
out vec3 myPos;

void main(void)
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	myPos = aPos;
	myColor = vec3(1.0,1.0,1.0);
}