#version 150

in  vec3 in_Position;
in  vec3 in_Normal;
out vec3 exNormal; // Phong
out vec3 exSurface; // Phong (specular)

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;


void main(void)
{
	exNormal = inverse(transpose(mat3(viewMatrix*modelMatrix))) * in_Normal; // Phong, "fake" normal transformation

	exSurface = vec3(viewMatrix * modelMatrix * vec4(in_Position, 1.0)); // Don't include projection here - we only want to go to view coordinates

	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0); // This should include projection
}
