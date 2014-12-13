#version 150
// bump mapping should be calculated
// 1) in view coordinates
// 2) in texture coordinates

in vec2 outTexCoord;
in vec3 out_Normal;
in vec3 Ps;
in vec3 Pt;


//for my cool lightsuff
in vec4 eye;
in vec3 pixPos;  // Needed for specular reflections


uniform sampler2D texUnit;
uniform vec3 camPosition;
uniform vec3 lightPosition;

uniform mat4 viewMatrix;

out vec4 out_Color;

float getMagnitude(vec3 v){
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void main(void)
{
    //Model stuff
    vec4 colorOfLight = vec4(1.0,1.0,1.0,1.0);
    vec4 colorDiffuseAlbedo = vec4(1.0,0.2, 1.0,1.0);

    // Light source in world coordinates --> view coordinates
    vec3 light = mat3(viewMatrix)*lightPosition;
    
    //point in view coordinates
    vec3 point = pixPos;
    
    //this assumes that both pixPos and Light are given in the same space
    vec3 L = light - point;

   float distance = getMagnitude(L);
   L =  normalize(L);
   L = L/(distance/7);


    vec3 Linv = -L;
    vec3 V = vec3(eye);
    V = normalize(V);
    //vec3 V = camPosition - pixPos;
    vec3 N = out_Normal;
    
    
    float fLTDistortion = 0.05, fLTScale = 5.0, fltAmbient = 0.05;
    int iLTPower = 2;
    
    vec4 fLightAttenuation = vec4(1.0,1.0,1.0,1.0); // TODO
    vec4 fLTThickness = vec4(1.0,1.0,1.0,1.0) - texture(texUnit, outTexCoord);
    
    
    // The relation to the light
    //Calculate the vector vLTLight = + vLight + vNormal * fLTDistortion
    vec3 vLTLight = L + N*fLTDistortion;
    
    //calc: pow(saturate(dot(vEye, -vLTLight)) , iLTPower) * fltScale
    float fLTDot = pow(dot(V, -vLTLight) / 5.0, iLTPower) * fLTScale;
    
    
    //fLT = flightAttenuation * ( fltDot + fltAmbient) * fltThickness
     vec4 fLT = fLightAttenuation * (fLTDot + fltAmbient)* fLTThickness;
    
    
     out_Color = colorOfLight*colorDiffuseAlbedo*fLT;

    // Test av fLT
    // out_Color = fLT;
    
    // Test av local thickness
    // out_Color = fLTThickness;
    
    // Test av backlight
    // out_Color = vec4(fLTDot, fLTDot, fLTDot, 1.0);
    
    // Test av L
    // out_Color = vec4(L, 1.0);
    
    // Test av V
    // out_Color = vec4(V, 1.0);
    
   // out_Color = vec4(1.0,0.0,0.0,1.0);
}



