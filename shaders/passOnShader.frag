#version 150

in vec2 TexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
    vec4 tempColor = texture(texUnit, TexCoord);
    
    out_Color = tempColor;
}
