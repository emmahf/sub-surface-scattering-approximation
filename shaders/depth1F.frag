#version 150

// Projected texture demo
// Based on a demo by Fabien Sanglard, http://www.fabiensanglard.net/shadowmapping/index.php

uniform sampler2D textureUnit;
in vec4 lightSourceCoord;
out vec4 out_Color;
uniform float shade;

void main()
{	
	// Perform perspective division to get the actual texture position
	vec4 shadowCoordinateWdivide = lightSourceCoord / lightSourceCoord.w;
	
	// Used to lower moire' pattern and self-shadowing
	// The optimal value here will vary with different GPU's depending on their Z buffer resolution.
	shadowCoordinateWdivide.z -= 0.002;
	
	// Look up the depth value
	float distanceFromLight = texture(textureUnit, shadowCoordinateWdivide.st).x;
	distanceFromLight = (distanceFromLight-0.5) * 2.0;
	
	out_Color = vec4(1.0 - pow(distanceFromLight,7));
	
//	out_Color =	 vec4(shadowCoordinateWdivide.z); // Bara avstånd

//	out_Color =	 vec4(distanceFromLight - shadowCoordinateWdivide.z);
//	out_Color =	 vec4((shadowCoordinateWdivide.z - distanceFromLight)*100.0 + 0.5);

/*
// Debugging - shadow map "raw"
	// 640x480
	float s = gl_FragCoord.x / 640.0;
	s = s * 2.0 - 1.0;
	float t = gl_FragCoord.y / 480.0;
	t = t * 2.0 - 1.0;
	vec2 st = vec2(s, t);
	float shadowMap = texture(textureUnit, st).x;
	out_Color =	 vec4(shadowMap);
*/
}
