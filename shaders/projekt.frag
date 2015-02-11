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
uniform vec3 objectDiffuseColor;
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


    //vec4 colorOfLight = vec4(colorOfLight,1.0);
    colorDiffuseAlbedo = vec4(objectDiffuseColor,1.0);

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


    float fLTDistortion = 0.1, fLTScale = 1.5, fltAmbient = 0.01;
    int iLTPower = 2;

    vec4 fLightAttenuation = vec4( vec3(1.0 - distance/20), 1.0); // TODO

    fLightAttenuation = vec4(1.0);
    float distanceToLightFromCamera = distance(vec4(0.0), vec4(lightPosition,1.0));
    float distanceToLight = distance(pixPos, lightPosition);

    vec4 fLTThickness = vec4(1.0,1.0,1.0,1.0) - texture(texUnit, outTexCoord);

    // The relation to the light
    //Calculate the vector vLTLight = + vLight + vNormal * fLTDistortion

    vec3 vLTLight = L + N*fLTDistortion;

    fLTThickness = fLTThickness*fLTThickness/3;

    //calc: pow(saturate(dot(vEye, -vLTLight)) , iLTPower) * fltScale
    float fLTDot = pow(dot(V, -vLTLight), iLTPower) * fLTScale;

    //fltThickness /= 2.0;
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

   //fLightAttenuation.w = 1.0;

   //Original textur
   //out_Color = texture(texUnit, outTexCoord);


   //Thickness
   //out_Color = fLTThickness;

   //No SSS
   //out_Color = vec4(0.0);

}
