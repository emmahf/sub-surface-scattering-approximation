#version 150


in vec3 in_Position;

out vec3 mvPosition;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(void)
{	
	vec4 pos = vec4(in_Position, 1.0);

	mvPosition = vec3(viewMatrix * modelMatrix * pos);

	gl_Position = projMatrix*viewMatrix*modelMatrix*pos;
}
