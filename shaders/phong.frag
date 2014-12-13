#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will follow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)

uniform vec3 lightPosition;
uniform mat4 viewMatrix;

void main(void)
{
    vec3 light = vec3(lightPosition.x, lightPosition.y, lightPosition.x); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
    light = mat3(viewMatrix)*lightPosition;
    light =  normalize(light);
	float diffuse, specular, shade;
	
	// Diffuse
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light
	
	// Specular
	vec3 r = reflect(-light, normalize(exNormal));
	vec3 v = normalize(-exSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = 2.0 * pow(specular, 3.0);
	specular = max(specular, 0.0);
	shade = 0.7*diffuse + 1.0*specular;
	
	outColor = vec4(shade, shade, shade, 1.0);

	vec3 diffuseLight = vec3(diffuse, diffuse, diffuse);

	vec4 colorDiffuseAlbedo = vec4(1.0,0.8,0.9,1.0);
        
    outColor = vec4(diffuseLight, 1.0) * colorDiffuseAlbedo;


}
