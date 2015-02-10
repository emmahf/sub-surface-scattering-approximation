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
uniform vec3 objectColor;
uniform vec3 lightColor;

float getMagnitude(vec3 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
void main(void)
{	
	mat3 test =  mat3(1.0);
    vec3 light = mat3(viewMatrix)*test*lightPosition;
    //vec3 light = lightPosition;
    //light = lightPosition;
    //light = lightPosition;
    //float norm = Norm(light);

    //L = from point to light
    vec3 L = light-exSurface;

    //Distance from light to point
    float distance = getMagnitude(L);

   	L =  normalize(L);
    L = L/(distance/2);

	float diffuse, specular, shade;

	//ambient
	vec3 amb = objectColor*lightColor*0.0;

	// Diffuse
	diffuse = dot(normalize(exNormal), L);
	diffuse = max(0.0, diffuse); // No negative light
	

	vec3 diff = objectColor * lightColor * diffuse*0.9;	   

	// Specular
	vec3 r = reflect(normalize(L),normalize(exNormal));
	vec3 v = normalize(exSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = pow(specular, 1.0);
	specular = max(specular, 0.0);

	vec3 spec = objectColor * lightColor *specular * 0.05;


	outColor = vec4(amb + diff + spec, 1.0);
	// vec4 colorDiffuseAlbedo = vec4(1.0,0.8,0.9,1.0);
        
 //    outColor = vec4(diffuseLight, 1.0) * colorDiffuseAlbedo;
 //    outColor[3] = 1.0;

}
