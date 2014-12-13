#version 150


// Glowing surfaces
in vec3 in_Position;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{	
	vec4 pos = vec4(in_Position, 1.0);
	gl_Position = projMatrix*viewMatrix*modelMatrix*pos;
}
