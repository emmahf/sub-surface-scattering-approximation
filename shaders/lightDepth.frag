#version 150

in vec3 mvPosition;

out vec4 out_Color;

uniform vec3 lightPosition;

void main(void)
{
    
    vec3 depthFromLight = (mvPosition - lightPosition);
    vec3 position = mvPosition;
    float depth = depthFromLight.z/20;
    out_Color = vec4(position,1.0);
}
