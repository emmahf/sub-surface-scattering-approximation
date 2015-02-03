#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform sampler2D texUnit2;
out vec4 out_Color;


float getMagnitude(vec4 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

void main(void)
{
    vec4 a = texture(texUnit, outTexCoord);
    vec4 b  = texture(texUnit2, outTexCoord);
	
    float d = distance(a, b);
	
    if(d > 0.0)
		d = 1.0 -d;
	out_Color = vec4(d,d,d,0);

}
