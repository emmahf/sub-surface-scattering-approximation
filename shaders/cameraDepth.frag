#version 150

in vec3 mvPosition;

out vec4 out_Color;

void main(void)
{
    
    float depth = -mvPosition.z/20;
    out_Color = vec4(mvPosition.x,mvPosition.y,mvPosition.z,1.0);
}
