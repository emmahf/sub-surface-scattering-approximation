//Emma Hesseborn Fagerholm
#ifdef __APPLE__

// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa

#else
	#ifdef WIN32
// MS
		#include <windows.h>
		#include <stdio.h>
		#include <GL/glew.h>
		#include <GL/glut.h>
	#else
// Linux
		#include <stdio.h>
		#include <GL/gl.h>
		#include "MicroGlut.h"
//		#include <GL/glut.h>
	#endif
#endif

#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "zpr.h"

// initial width and heights
#define W 680
#define H 680

#define NEAR 1.0
#define FAR 150.0
#define RIGHT 0.5
#define LEFT -0.5
#define TOP 0.5
#define BOTTOM -0.5

#define NUM_LIGHTS 4

void OnTimer(int value);



GLfloat square[] = {
                            -1,-1,0,
                            -1,1, 0,
                            1,1, 0,
                            1,-1, 0};
GLfloat squareTexCoord[] = {
                             0, 0,
                             0, 1,
                             1, 1,
                             1, 0};
GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};


mat4 projectionMatrix,
        viewMatrix, rotateMatrix, viewMatrixTemp; // viewMatrix controlled by zpr.c

mat4 modelMatrix = {{ 3.0, 0.0, 0.0, 4.3,
                              0.0, 3.0, 0.0, 2.3,
                              0.0, 0.0, 3.0, -1.0,
                              0.0, 0.0, 0.0, 1.0}};

mat4 statueMatrix = {{ 2.5, 0.0, 0.0, 0.0,
                              0.0, 2.5, 0.0, -2.0,
                              0.0, 0.0, 2.5, 0.0,
                              0.0, 0.0, 0.0, 1.0}};


mat4 boxMatrix = {{ 1.5, 0.0, 0.0, 0.0,
                              0.0, 1.5, 0.0, -2.0,
                              0.0, 0.0, 1.5, -5.0,
                              0.0, 0.0, 0.0, 1.0}};



mat4 sceneModelMatrix = {{ 1.0, 0.0, 0.0, -1.0,
                            0.0, 1.0, 0.0, -3.0,
                            0.0, 0.0, 1.0, -10.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 sphereModelMatrix = {{ 4.0, 0.0, 0.0, 0.0,
                            0.0, 4.0, 0.0, 0.0,
                            0.0, 0.0, 4.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 bottomModelMatrix = {{ 4.0, 0.0, 0.0, 0.0,
                            0.0, 4.0, 0.0, -3.0,
                            0.0, 0.0, 4.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};


mat4 side1ModelMatrix = {{ 4.0, 0.0, 0.0,  -9.0,
                            0.0, 4.0, 0.0, 5.0,
                            0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 side2ModelMatrix = {{ 4.0, 0.0, 0.0,  0.0,
                            0.0, 4.0, 0.0, 5.0,
                            0.0, 0.0, 1.0, -10.0,
                            0.0, 0.0, 0.0, 1.0}};


mat4 lightMatrix = {{ 1.0, 0.0, 0.0,  0.0,
                            0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0, -9.0,
                            0.0, 0.0, 0.0, 1.0}};


//----------------------Globals-------------------------------------------------
Point3D axis, cam, point, lightPosition, originalLightPosition, lightColor, bunnyColor, sceneColor, cameraDepth;
Model *bunny, *squareModel, *scene, *sphere,
      *bottom, *side1, *side2,
      *statue,
      *box, *box_stretched, *box_bulge, *box_valley;

FBOstruct *fbo_depth, *fbo2, *fbo3, *fbo_cameraDepth, *fbo_lightDepth;
GLuint phongshader = 0,
        shader = 0, passShader = 0 , joinshader = 0,
        lightShader = 0,
        cameraDepthShader = 0, lightDepthShader = 0, thickness = 0;

GLuint thicknessBunny, thicknessStatue, thicknessBox;
int count = 0;
double moveX, moveValue;
unsigned int vsBuffer, vtBuffer; // Attribute buffers for Vs and Vt

//-------------------------------------------------------------------------------------


//All model matrixes must be added here when updating
void updateScene(){

    modelMatrix = Mult(sceneModelMatrix, modelMatrix);
    bottomModelMatrix = Mult(sceneModelMatrix, bottomModelMatrix);
    side1ModelMatrix = Mult(sceneModelMatrix, side1ModelMatrix);
    side2ModelMatrix = Mult(sceneModelMatrix, side2ModelMatrix);
    sphereModelMatrix = Mult(sceneModelMatrix, sphereModelMatrix);
    statueMatrix = Mult(sceneModelMatrix, statueMatrix);
    boxMatrix = Mult(sceneModelMatrix, boxMatrix);
}

void init(void)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // Load shader
    shader = loadShaders("shaders/projekt.vert", "shaders/projekt.frag");
    phongshader = loadShaders("shaders/phong.vert", "shaders/phong.frag");
    passShader = loadShaders("shaders/plaintextureshader.vert", "shaders/plaintextureshader.frag");
    joinshader = loadShaders("shaders/joinshader.vert", "shaders/joinshader.frag");
    lightShader = loadShaders("shaders/illuminati.vert", "shaders/illuminati.frag");
    cameraDepthShader = loadShaders("shaders/cameraDepth.vert", "shaders/cameraDepth.frag");
    lightDepthShader = loadShaders("shaders/lightDepth.vert", "shaders/lightDepth.frag");
    thickness = loadShaders("shaders/thickness.vert", "shaders/thickness.frag");

    // Init FBOs
	fbo_depth = initFBO(W, H, 0);
	fbo2 = initFBO(W, H, 0);
	fbo3 = initFBO(W, H, 0);
    fbo_cameraDepth = initFBO(W,H,0);
    fbo_lightDepth = initFBO(W,H,0);


    squareModel = LoadDataToModel(
            square, NULL, squareTexCoord, NULL,
            squareIndices, 4, 6);

    cam = SetVector(0, 0, 0.01);
    point = SetVector(0, 0, 10);
    axis = SetVector(0, 1, 0);


	// load the model
    bunny = LoadModelPlus("objects/bunny2/bunny_unwrap_noextras_blender.obj");
    LoadTGATextureSimple("textures/badBunny.tga", &thicknessBunny);
    modelMatrix = Mult( modelMatrix, ArbRotate(axis, -0.6));
    statue = LoadModelPlus("good_objects/statue_unwrapped_blender.obj");
    LoadTGATextureSimple("textures/statue.tga", &thicknessStatue);

    box = LoadModelPlus("good_objects/box_standard_blender.obj");
    LoadTGATextureSimple("textures/box.tga", &thicknessBox);

    box_bulge = LoadModelPlus("good_objects/box_bulge.obj");
    box_valley = LoadModelPlus("good_objects/box_valley.obj");
    box_stretched = LoadModelPlus("good_objects/box_stretched.obj");

    // load the scenemodels
    bottom = LoadModelPlus("objects/bottom.obj");
    side1 = LoadModelPlus("objects/side1.obj");
    side2 = LoadModelPlus("objects/side2.obj");

    // load sphere
    sphere = LoadModelPlus("objects/sphere.obj");
    printf("%d vertices\n", sphere->numVertices);
    printf("%d indices\n", sphere->numIndices);

    //Light stuff
    lightPosition = SetVector(-5.0,2.0,-4.0);
    sphereModelMatrix = Mult(T(lightPosition.x, lightPosition.y, lightPosition.z), sphereModelMatrix);
    lightColor = SetVector(1.0,1.0,1.0);

    //Colors
    bunnyColor = SetVector(1.0,0.4,1.0);
    sceneColor = SetVector(0.2,0.2,0.7);

    glutTimerFunc(5, &OnTimer, 0);

    moveValue = 0.005;
    moveX = moveValue;

	zprInit(&viewMatrix, cam, point);

}

void OnTimer(int value)
{
   // sceneModelMatrix = Mult(sceneModelMatrix, T(moveX,0.0,0.0));
   // lightPosition.x += moveX;
    count++;

    sphereModelMatrix = Mult(sphereModelMatrix, T(moveX, 0.0, 0.0) );
    lightMatrix = Mult(lightMatrix, T(moveX,0.0,0.0));

    if(count % 900 == 0){
        moveX = -moveX;
       // printf("lightPosition.x %f \n", lightPosition.x);
    }


    //printf("lightPosition.x %f \n", lightPosition.x);

	glutPostRedisplay();
	glutTimerFunc(5, &OnTimer, value);
}

//-------------------------------callback functions------------------------------------------


void useThicknessShader(FBOstruct *fbo){


}

void drawSinglePhongObject(Model * model, mat4 m,  GLfloat r, GLfloat g, GLfloat b){

    glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelMatrix"), 1, GL_TRUE, m.m);
    glUniform3f(glGetUniformLocation(phongshader,"objectColor"), r,g,b);

    DrawModel(model, phongshader, "in_Position", "in_Normal", NULL);

}

void drawSingleTranslucentObject(GLuint thicknessTex, Model * model, mat4 m, GLfloat r, GLfloat g, GLfloat b){
    glBindTexture(GL_TEXTURE_2D, thicknessTex);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_TRUE, m.m);
    glUniform3f(glGetUniformLocation(shader,"objectDiffuseColor"), r,g,b);

    DrawModel(model, shader, "in_Position", "in_Normal", "in_TexCoord");
}


void display(void)
{

    //Update matrices and light to scene transformation

    updateScene();

    lightPosition = SetVector(sphereModelMatrix.m[3],sphereModelMatrix.m[7],sphereModelMatrix.m[11]);


    // //FRONT Z
    // useFBO(fbo_cameraDepth, 0L, 0L);
    // glUseProgram(cameraDepthShader);
    // glClearColor(0.0, 0.0, 0.0, 1.0);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // //Send info that is the same for all objects (projection, view matrix, light etc)
    // glUniformMatrix4fv(glGetUniformLocation(cameraDepthShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    // glUniformMatrix4fv(glGetUniformLocation(cameraDepthShader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);


    // //Enable depth tests and all that
    // // Enable Z-buffering
    // glEnable(GL_DEPTH_TEST);
    // // Enable backface culling
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    // //bunny
    // glUniformMatrix4fv(glGetUniformLocation(cameraDepthShader, "modelMatrix"), 1, GL_TRUE, modelMatrix.m);
    // DrawModel(bunny, cameraDepthShader, "in_Position", NULL, NULL);


    // //LIGHT Z
    // // vec3 camtemp = SetVector(cam.x, cam.y, cam.z);
    // // cam = SetVector(lightPosition.x, lightPosition.y, lightPosition.z);
    // // zprInit(&viewMatrix, cam, point);

    // useFBO(fbo_lightDepth, 0L, 0L);
    // glUseProgram(lightDepthShader);
    // glClearColor(0.0, 0.0, 0.0, 1.0);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // //Send info that is the same for all objects (projection, view matrix, light etc)
    // glUniformMatrix4fv(glGetUniformLocation(lightDepthShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    // glUniformMatrix4fv(glGetUniformLocation(lightDepthShader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
    // glUniform3f(glGetUniformLocation(lightDepthShader,"lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

    // //Enable depth tests and all that
    // // Enable Z-buffering
    // glEnable(GL_DEPTH_TEST);
    // // Enable backface culling
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);

    // //bunny
    // glUniformMatrix4fv(glGetUniformLocation(lightDepthShader, "modelMatrix"), 1, GL_TRUE, modelMatrix.m);
    // DrawModel(bunny, lightDepthShader, "in_Position", NULL, NULL);

    // // cam = SetVector(camtemp.x, camtemp.y, camtemp.z);
    // // zprInit(&viewMatrix, camtemp, point);

    // //THICKNESS SHADER
    // useFBO(fbo3, fbo_cameraDepth, fbo_lightDepth); //write to fbo3, read from fbo2 and from fbo1
    // glUseProgram(thickness);

    // glUniform1i(glGetUniformLocation(thickness, "texUnit"), 0);
    // glUniform1i(glGetUniformLocation(thickness, "texUnit2"), 1);

    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);

    // DrawModel(squareModel, thickness, "in_Position", NULL, "in_TexCoord");

    //DRAW  __ALL__ PHONG OBJECTS TO THE SAME FBO (+ Depth test! )
    useFBO(fbo_depth,0L,0L);

    glUseProgram(phongshader);
    glClearColor(0.2, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Send info that is the same for all objects (projection, view matrix, light etc)
    glUniformMatrix4fv(glGetUniformLocation(phongshader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

    glUniformMatrix4fv(glGetUniformLocation(phongshader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
    glUniform3f(glGetUniformLocation(phongshader, "camPosition"),cam.x,cam.y,cam.z);
    glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);
    glUniform3f(glGetUniformLocation(phongshader,"lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(glGetUniformLocation(phongshader,"lightColor"), lightColor.x, lightColor.y, lightColor.z);


    //Enable depth tests and all that
    // Enable Z-buffering
    glEnable(GL_DEPTH_TEST);
    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    drawSinglePhongObject(bunny, modelMatrix, bunnyColor.x,bunnyColor.y, bunnyColor.z);
    drawSinglePhongObject(statue, statueMatrix, 0.3,0.7,0.4);
    drawSinglePhongObject(box, boxMatrix, 1.0,0.2,0.5);
    drawSinglePhongObject(bottom, bottomModelMatrix, sceneColor.x, sceneColor.y, sceneColor.z);
    drawSinglePhongObject(side1, side1ModelMatrix, sceneColor.x, sceneColor.y, sceneColor.z);
    drawSinglePhongObject(side2, side2ModelMatrix, sceneColor.x, sceneColor.y, sceneColor.z);

    //ALL LIGHTSURFACES (SAME FBO as PHONG)
    glUseProgram(lightShader);
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);

    //sphere
    glUniformMatrix4fv(glGetUniformLocation(lightShader, "modelMatrix"), 1, GL_TRUE, sphereModelMatrix.m);
    DrawModel(sphere, lightShader, "in_Position", NULL, NULL);


    //DRAW ALL TRANSLUCENT OBJECTS TO ANOTHER FBO
    useFBO(fbo2, 0L, 0L);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
    glUniform3f(glGetUniformLocation(shader, "camPosition"),cam.x,cam.y,cam.z);
    glUniform3f(glGetUniformLocation(shader,"lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform1i(glGetUniformLocation(shader, "texUnit"), 0);

    // Enable Z-buffering
    glDisable(GL_CULL_FACE);

    //bunny
    drawSingleTranslucentObject(thicknessBunny , bunny, modelMatrix, bunnyColor.x,bunnyColor.y, bunnyColor.z);
    drawSingleTranslucentObject(thicknessStatue, statue, statueMatrix, 0.3,0.7,0.4);
    drawSingleTranslucentObject(thicknessBox, box, boxMatrix, 1.0,0.2,0.5);


    //JOIN SHADER
    useFBO(fbo3, fbo_depth, fbo2); //write to fbo3, read from fbo2 and from fbo1
    glUseProgram(joinshader);

    glUniform1i(glGetUniformLocation(joinshader, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(joinshader, "texUnit2"), 1);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    DrawModel(squareModel, joinshader, "in_Position", NULL, "in_TexCoord");

    //Pass on Shader
    useFBO(0L, fbo3, 0L);
    glClearColor(0.0, 0.0, 0.0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(passShader);
    glUniform1i(glGetUniformLocation(passShader, "texUnit"), 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    DrawModel(squareModel, passShader, "in_Position", NULL, "in_TexCoord");


    //Reset scene transformation
    sceneModelMatrix = IdentityMatrix();
}

void reshape(GLsizei w, GLsizei h)
{
    glViewport(0, 0, w, h);
    GLfloat ratio = (GLfloat) w / (GLfloat) h;
    projectionMatrix = perspective(70, ratio, 0.2, 1000.0);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
}


void myKeys(unsigned char key, int x, int y)
{
  switch (key)
   {
   case 'w':
     sceneModelMatrix = Mult(sceneModelMatrix, T(0.0,0.0,0.2));
     break;
   case 's':
     sceneModelMatrix = Mult(sceneModelMatrix, T(0.0,0.0,-0.2));
     break;
   case 'j':
    sceneModelMatrix = Mult(sceneModelMatrix, ArbRotate(axis, 0.3));
    break;
  case 'k':
    sceneModelMatrix = Mult(sceneModelMatrix,ArbRotate(axis, -0.3));
    break;

   default:
     break;
    }
}


//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitContextVersion(3, 2); // Might not be needed in Linux
    glutInitWindowSize(W, H);
    glutCreateWindow ("Sub Surface Scattering Approximation");
    glutDisplayFunc(display);

    glutTimerFunc(5, &OnTimer, 0);
    glutReshapeFunc(reshape);

    init();
    zprInit(&viewMatrix, cam, point);
    glutKeyboardFunc(myKeys);

    glutMainLoop();
    exit(0);
}
