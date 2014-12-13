#version 150

in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 exNormal; // Phong
out vec3 exSurface; // Phong (specular)
//out vec3 pixPos;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

void main(void)
{	

	exNormal = transpose(inverse(mat3(viewMatrix*modelMatrix))) * in_Normal; // Phong, "fake" normal transformation
	//exNormal = normalize(gl_NormalMatrix * in_Normal);
	//exNormal = mat3(viewMatrix)*in_Normal;
	//exNormal = vec3(1.0,0.0,0.0);
	//exNormal = transpose(inverse(viewMatrix*modelMatrix)) * vec4(in_Normal,1.0);
	exSurface = vec3(viewMatrix * modelMatrix * vec4(in_Position, 1.0)); // Don't include projection here - we only want to go to view coordinates

	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0); // This should include projection
}
