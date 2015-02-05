#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec3 inTexCoord;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

void main(void)
{
    gl_Position = projMatrix * viewMatrix * modelMatrix*vec4(in_Position, 1.0);
}
