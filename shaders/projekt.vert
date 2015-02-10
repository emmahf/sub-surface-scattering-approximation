#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;
in vec3 Vs;
in vec3 Vt;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

out vec2 outTexCoord;
out vec3 out_Normal;
out vec3 Ps;
out vec3 Pt;
out vec3 pixPos;  // Needed for specular reflections

//eye vector = vector from point to camera, cam - point, in camera space. since camera is a 0 in camera space this becomes: eye = -viewmodel*position

out vec4 eye;

void main(void)
{
    outTexCoord = in_TexCoord;
    out_Normal = mat3(viewMatrix) * mat3(modelMatrix) * in_Normal; // Cheated normal matrix, OK with no non-uniform scaling

    Ps = normalize( mat3(viewMatrix) * mat3(modelMatrix) * Vs );
    Pt = normalize( mat3(viewMatrix) * mat3(modelMatrix) * Vt );

    //vector from point to camera in viewspace
    eye = -viewMatrix * modelMatrix * vec4(in_Position,1.0);

    //the point in viewspace
    pixPos = vec3(viewMatrix * modelMatrix * vec4(in_Position, 1.0));



    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
}
