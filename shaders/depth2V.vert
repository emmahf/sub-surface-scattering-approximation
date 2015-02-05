#version 150

// Projected texture demo
// Based on a demo by Fabien Saglard, http://www.fabiensanglard.net/shadowmapping/index.php

out vec4 lightSourceCoord;
uniform int texunit;

in vec3 in_Position;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 textureMatrix;

void main()
{
	lightSourceCoord = textureMatrix * vec4(in_Position, 1.0); // Transform vertex to light source coordinates
	gl_Position = projMatrix * viewMatrix *  modelMatrix * vec4(in_Position, 1.0);
}
