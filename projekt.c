// Bump mapping lab by Ingemar
// Revised 2013 to use MicroGlut, VectorUtils3 and zpr

// gcc lab1-2.c ../common/*.c -lGL -o lab1-2 -I../common

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
#define W 512
#define H 512

#define NEAR 1.0
#define FAR 150.0
#define RIGHT 0.5
#define LEFT -0.5
#define TOP 0.5
#define BOTTOM -0.5

#define NUM_LIGHTS 4

#define TEX_UNIT 0
#define TEX_UNIT1 1


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
        viewMatrix, rotateMatrix, viewMatrixTemp,
        textureMatrix, lightViewMatrix; // viewMatrix controlled by zpr.c
mat4 modelMatrix = {{ 3.0, 0.0, 0.0, 0.0,
                              0.0, 3.0, 0.0, 0.0,
                              0.0, 0.0, 3.0, 0.0,
                              0.0, 0.0, 0.0, 1.0}};

mat4 sceneModelMatrix = {{ 1.0, 0.0, 0.0, 0.0,
                            0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0, 1.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 sphereModelMatrix = {{ 1.0, 0.0, 0.0, 0.0,
                            0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 bottomModelMatrix = {{ 1.0, 0.0, 0.0, 0.0,
                            0.0, 1.0, 0.0, -1.9,
                            0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};


mat4 side1ModelMatrix = {{ 1.0, 0.0, 0.0,  -9.0,
                            0.0, 1.0, 0.0, 5.0,
                            0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 side2ModelMatrix = {{ 1.0, 0.0, 0.0,  0.0,
                            0.0, 1.0, 0.0, 5.0,
                            0.0, 0.0, 1.0, -10.0,
                            0.0, 0.0, 0.0, 1.0}};
mat4 sbmat = {{ 0.5, 0.0, 0.0,  0.5,
                            0.0, 0.5, 0.0, 0.5,
                            0.0, 0.0, 0.5, 0.5,
                            0.0, 0.0, 0.0, 1.0}};
//----------------------Globals-------------------------------------------------
Point3D cam, point, lightPosition, lightColor, bunnyColor, sceneColor, cameraDepth, axis;


Model *bunny, *squareModel, *scene, *sphere, *bottom, *side1, *side2;
FBOstruct *fbo_depth, *fbo_depth, *fbo_depth2, *fbo3, *fbo_cameraDepth, *fbo_lightDepth;
GLuint phongshader = 0,
        shader = 0, passShader = 0 , joinshader = 0,
        lightShader = 0,
        cameraDepthShader = 0, lightDepthShader = 0, thickness = 0;

GLuint projTexShaderId,projTexShaderId2, plainShaderId;
GLuint projTexMapUniform;

int isMoving = 1;

GLuint thicknessTex;
double moveX, moveValue;
unsigned int vsBuffer, vtBuffer; // Attribute buffers for Vs and Vt

//-------------------------------------------------------------------------------------

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


    //shadow shaders

    projTexShaderId = loadShaders("shaders/depth1V.vert", "shaders/depth1F.frag");
    projTexShaderId2 = loadShaders("shaders/depth2V.vert", "shaders/depth2F.frag");
    projTexMapUniform = glGetUniformLocation(projTexShaderId,"textureUnit");
    plainShaderId = loadShaders("shaders/plain.vert", "shaders/plain.frag");


    // Init FBOs
	fbo_depth = initFBO(W, H, 0);
	//fbo2 = initFBO(W, H, 0);
    fbo_depth = initFBO2(W,H, 0, 1);
    fbo_depth2 = initFBO2(W,H, 0, 1);
	fbo3 = initFBO(W, H, 0);
    fbo_cameraDepth = initFBO(W,H,0);
    fbo_lightDepth = initFBO(W,H,0);


    squareModel = LoadDataToModel(
            square, NULL, squareTexCoord, NULL,
            squareIndices, 4, 6);

	// load the model
    bunny = LoadModelPlus("objects/bunny2/bunny_unwrap_noextras_blender.obj");
    printf("%d vertices\n", bunny->numVertices);
    printf("%d indices\n", bunny->numIndices);


    // Load thickness map
    LoadTGATextureSimple("objects/bunny2/bunny_unwrap_middle.tga", &thicknessTex);

	// load the scenemodel
    scene = LoadModelPlus("objects/scene_big.obj");
    printf("%d vertices\n", scene->numVertices);
    printf("%d indices\n", scene->numIndices);

    // load the scenemodel
    bottom = LoadModelPlus("objects/bottom.obj");
    printf("%d vertices\n", bottom->numVertices);
    printf("%d indices\n", bottom->numIndices);

    // load the scenemodel
    side1 = LoadModelPlus("objects/side1.obj");
    printf("%d vertices\n", side1->numVertices);
    printf("%d indices\n", side1->numIndices);

    // load the scenemodel
    side2 = LoadModelPlus("objects/side2.obj");
    printf("%d vertices\n", side2->numVertices);
    printf("%d indices\n", side2->numIndices);

    // load sphere
    sphere = LoadModelPlus("objects/sphere.obj");
    printf("%d vertices\n", sphere->numVertices);
    printf("%d indices\n", sphere->numIndices);

    cam = SetVector(1, 1, 5);
    point = SetVector(0, 0, -10);

    //Light stuff
    lightPosition = SetVector(0.0,0.0,-9.0);
    sphereModelMatrix = T(lightPosition.x, lightPosition.y, lightPosition.z);
	lightColor = SetVector(1.0,1.0,0.8);
    axis = SetVector(0,1,0);

    //Colors
    bunnyColor = SetVector(1.0,0.4,1.0);
    sceneColor = SetVector(0.6,0.6,1.0);

    glutTimerFunc(5, &OnTimer, 0);

    moveValue = 0.02;
    moveX = moveValue;

	zprInit(&viewMatrix, cam, point);

}

void OnTimer(int value)
{
    if(isMoving){
   // sceneModelMatrix = Mult(sceneModelMatrix, T(moveX,0.0,0.0));
    lightPosition.x += moveX;
    sphereModelMatrix = Mult(sphereModelMatrix, T(moveX, 0.0, 0.0) );

    if(lightPosition.x < -8.0){
        moveX = moveValue;
        printf("lightPosition.x %f \n", lightPosition.x);
    }
    else if(lightPosition.x > 8.0){
        moveX = -moveValue;
        printf("lightPosition.x %f \n", lightPosition.x);
    }

    //printf("lightPosition.x %f \n", lightPosition.x);
    }
	glutPostRedisplay();
	glutTimerFunc(5, &OnTimer, value);
}

//-------------------------------callback functions------------------------------------------


void useThicknessShader(FBOstruct *fbo){


}

void setTextureMatrix(mat4 currentModelMatrix)
{
    mat4 scaleBiasMatrix;

    IdentityMatrix(textureMatrix);

// Scale and bias transform, moving from unit cube [-1,1] to [0,1]
    scaleBiasMatrix = Mult(T(0.5, 0.5, 0.0), S(0.5, 0.5, 1.0));
    textureMatrix = Mult(Mult(scaleBiasMatrix, projectionMatrix), Mult(lightViewMatrix,currentModelMatrix));


    //  textureMatrix = Mult(Mult(scaleBiasMatrix, projectionMatrix), modelViewMatrix);
    // Multiply modelview and transformation matrices
}

void drawObjects(GLuint shaderId){

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
    glUniform3f(glGetUniformLocation(shaderId,"lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

    //bunny
    setTextureMatrix(modelMatrix);
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.9); // Brighter objects
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, modelMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
    glUniform3f(glGetUniformLocation(shaderId,"objectColor"), bunnyColor.x, bunnyColor.y, bunnyColor.z);

    DrawModel(bunny, shaderId, "in_Position", NULL, NULL);

    //scene
    setTextureMatrix(bottomModelMatrix);
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.5); // Brighter objects
    glUniform3f(glGetUniformLocation(shaderId,"objectColor"), sceneColor.x, sceneColor.y, sceneColor.z);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, bottomModelMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
    DrawModel(bottom, shaderId, "in_Position", NULL,NULL);

    setTextureMatrix(side1ModelMatrix);
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.5); // Brighter objects
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, side1ModelMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
    DrawModel(side1, shaderId, "in_Position", NULL, NULL);


    setTextureMatrix(side2ModelMatrix);
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.5); // Brighter objects
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, side2ModelMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
    DrawModel(side2, shaderId, "in_Position", NULL, NULL);

}


void drawObjectsFirstPass(GLuint shaderId){

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);

    //bunny
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.9); // Brighter objects
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, modelMatrix.m);
    glUniform3f(glGetUniformLocation(shaderId,"objectColor"), bunnyColor.x, bunnyColor.y, bunnyColor.z);

    DrawModel(bunny, shaderId, "in_Position", NULL, NULL);

    //scene
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.5); // Brighter objects
    glUniform3f(glGetUniformLocation(shaderId,"objectColor"), sceneColor.x, sceneColor.y, sceneColor.z);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, bottomModelMatrix.m);
    DrawModel(bottom, shaderId, "in_Position", NULL,NULL);

    setTextureMatrix(side1ModelMatrix);
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.5); // Brighter objects
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, side1ModelMatrix.m);
    DrawModel(side1, shaderId, "in_Position", NULL, NULL);


    setTextureMatrix(side2ModelMatrix);
    glUniform1f(glGetUniformLocation(shaderId, "shade"), 0.5); // Brighter objects
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMatrix"), 1, GL_TRUE, side2ModelMatrix.m);
    DrawModel(side2, shaderId, "in_Position", NULL, NULL);

}


void display(void)
{

    //Update matrices and light to scene transformation
    modelMatrix = Mult(sceneModelMatrix, modelMatrix);
    bottomModelMatrix = Mult(sceneModelMatrix, bottomModelMatrix);
    side1ModelMatrix = Mult(sceneModelMatrix, side1ModelMatrix);
    side2ModelMatrix = Mult(sceneModelMatrix, side2ModelMatrix);
    sphereModelMatrix = Mult(sceneModelMatrix, sphereModelMatrix);

    lightPosition = MultVec3(sceneModelMatrix, lightPosition);


    vec3 cameraPosition = cam;
    // ----- Drawing from point of light

    //save old camera position
    //vec3 camtemp = SetVector(cam.x, cam.y, cam.z);

    //set camera position to the light position
    //cam = SetVector(lightPosition.x, lightPosition.y, lightPosition.z);

    //create the new viewmatrix accordingly.
    //zprInit(&viewMatrix, cam, point);

    // Setup projection matrix
   // projectionMatrix = perspective(45, W/H, 2, 4000);

    //Create the view matrix from the light
    lightViewMatrix = lookAt(lightPosition.x, lightPosition.y, lightPosition.z,
                point.x, point.y, point.z, 0,1,0);

    // Setup the view from the light source
    viewMatrix = lightViewMatrix;

    //Setup projection matrix

    // 1. render scene to FBO 1 with Back culling
    useFBO(fbo_depth, NULL, NULL);
    glViewport(0,0,W,H);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE); // Depth only
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Using the simple shader
    glUseProgram(plainShaderId);
    glUniform1i(plainShaderId,TEX_UNIT);
    glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
    glBindTexture(GL_TEXTURE_2D,0);

    glCullFace(GL_BACK);

    printf("%s\n", "mohahahaha");
    drawObjectsFirstPass(plainShaderId);


    // 1. render scene to FBO 2
    useFBO(fbo_depth2, NULL, NULL);
    glViewport(0,0,W,H);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE); // Depth only
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Using the simple shader with front culling
    glUseProgram(plainShaderId);
    glUniform1i(plainShaderId,TEX_UNIT);
    glActiveTexture(GL_TEXTURE1 + TEX_UNIT);
    glBindTexture(GL_TEXTURE_2D,1);

    glCullFace(GL_FRONT);

    printf("%s\n", "mohahahaha");
    drawObjectsFirstPass(plainShaderId);


    // Render from Camera

    //Move camera back to original camera position
    //zprInit(&viewMatrix, camtemp,point);
    useFBO(NULL, fbo_depth, fbo_depth2);
    glViewport(0,0,W,H);

    //Enabling color write (previously disabled for light POV z-buffer rendering)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Clear previous frame values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Using the projTex shader
    glUseProgram(projTexShaderId2);

    glUniform1i(glGetUniformLocation(projTexShaderId2, "texxxUnit"), 0);
    glUniform1i(glGetUniformLocation(projTexShaderId2, "texxxUnit2"), 1);


    glUniform1i(projTexMapUniform,TEX_UNIT);
    glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
    glBindTexture(GL_TEXTURE_2D,fbo_depth->depth);

    glUniform1i(projTexMapUniform,TEX_UNIT);
    glActiveTexture(GL_TEXTURE1 + TEX_UNIT);
    glBindTexture(GL_TEXTURE_2D,fbo_depth2->depth);



    // Setup the modelview from the camera
   // modelViewMatrix = lookAt(p_camera.x, p_camera.y, p_camera.z,
    //                l_camera.x, l_camera.y, l_camera.z, 0,1,0);

        // Setup the view from the light source
   viewMatrix = lookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
               point.x, point.y, point.z, 0,1,0);


       printf("%s\n", "yakiyakiyaki");
    glCullFace(GL_BACK);

    drawObjects(projTexShaderId2);

    glutSwapBuffers();



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



/*
*
*
*       WORKING CODE FOR PART 1 (I.E Do not touch!)
*
*/
    //DRAW  __ALL__ PHONG OBJECTS TO THE SAME FBO (+ Depth test! )
 //    useFBO(fbo_depth,0L,0L);

 //    glUseProgram(phongshader);
 //    glClearColor(0.2, 0.0, 0.0, 1.0);
 //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 //    //Send info that is the same for all objects (projection, view matrix, light etc)
 //    glUniformMatrix4fv(glGetUniformLocation(phongshader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

 //    glUniformMatrix4fv(glGetUniformLocation(phongshader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
 //    glUniform3f(glGetUniformLocation(phongshader, "camPosition"),cam.x,cam.y,cam.z);
 //    glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);
 //    glUniform3f(glGetUniformLocation(phongshader,"lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
 //    glUniform3f(glGetUniformLocation(phongshader,"lightColor"), lightColor.x, lightColor.y, lightColor.z);


 //    //Enable depth tests and all that
 //    // Enable Z-buffering
 //    glEnable(GL_DEPTH_TEST);
 //    // Enable backface culling
 //    glEnable(GL_CULL_FACE);
 //    glCullFace(GL_BACK);

 //    //bunny
 //    glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelMatrix"), 1, GL_TRUE, modelMatrix.m);
 //    glUniform3f(glGetUniformLocation(phongshader,"objectColor"), bunnyColor.x, bunnyColor.y, bunnyColor.z);

 //    DrawModel(bunny, phongshader, "in_Position", "in_Normal", NULL);

 //    //scene
 //    glUniform3f(glGetUniformLocation(phongshader,"objectColor"), sceneColor.x, sceneColor.y, sceneColor.z);
 //    glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelMatrix"), 1, GL_TRUE, bottomModelMatrix.m);
 //    DrawModel(bottom, phongshader, "in_Position", "in_Normal", "in_TexCoord");

 //    glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelMatrix"), 1, GL_TRUE, side1ModelMatrix.m);
 //    DrawModel(side1, phongshader, "in_Position", "in_Normal", "in_TexCoord");

 //    glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelMatrix"), 1, GL_TRUE, side2ModelMatrix.m);
 //    DrawModel(side2, phongshader, "in_Position", "in_Normal", "in_TexCoord");

 //    //ALL LIGHTSURFACES (SAME FBO as PHONG)
 //    glUseProgram(lightShader);
 //    glUniformMatrix4fv(glGetUniformLocation(lightShader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
 //    glUniformMatrix4fv(glGetUniformLocation(lightShader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);

 //    //sphere
 //    glUniformMatrix4fv(glGetUniformLocation(lightShader, "modelMatrix"), 1, GL_TRUE, sphereModelMatrix.m);
 //    DrawModel(sphere, lightShader, "in_Position", NULL, NULL);


 //    //DRAW ALL TRANSLUCENT OBJECTS TO ANOTHER FBO
 //    useFBO(fbo2, 0L, 0L);
 //    glBindTexture(GL_TEXTURE_2D, thicknessTex);
	// //glClearColor(0.1, 0.1, 0.3, 0);
 //    glClearColor(0.0, 0.0, 0.0, 1.0);
 //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 //    glUseProgram(shader);

 //    glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
 //    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
 //    glUniform3f(glGetUniformLocation(shader, "camPosition"),cam.x,cam.y,cam.z);
 //    glUniform3f(glGetUniformLocation(shader,"lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
 //    glUniform1i(glGetUniformLocation(shader, "texUnit"), 0);

 //    // Enable Z-buffering
 //    glDisable(GL_CULL_FACE);
 //    //glDisable(GL_DEPTH_TEST);
 //    // Enable backface culling
 //    //glEnable(GL_CULL_FACE);
 //    //glCullFace(GL_BACK);

 //    //bunny
 //    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_TRUE, modelMatrix.m);
 //    DrawModel(bunny, shader, "in_Position", "in_Normal", "in_TexCoord");


 //    //JOIN SHADER
 //    useFBO(fbo3, fbo_depth, fbo2); //write to fbo3, read from fbo2 and from fbo1
 //    glUseProgram(joinshader);

 //    glUniform1i(glGetUniformLocation(joinshader, "texUnit"), 0);
 //    glUniform1i(glGetUniformLocation(joinshader, "texUnit2"), 1);

 //    glDisable(GL_CULL_FACE);
 //    glDisable(GL_DEPTH_TEST);
 //    DrawModel(squareModel, joinshader, "in_Position", NULL, "in_TexCoord");

 //    //Pass on Shader
 //    useFBO(0L, fbo3, 0L);
 //    glClearColor(0.0, 0.0, 0.0, 0);
 //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 //    glUseProgram(passShader);
 //    glUniform1i(glGetUniformLocation(passShader, "texUnit"), 0);
 //    glDisable(GL_CULL_FACE);
 //    glDisable(GL_DEPTH_TEST);
 //    DrawModel(squareModel, passShader, "in_Position", NULL, "in_TexCoord");


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
     sceneModelMatrix = Mult(sceneModelMatrix, ArbRotate(axis, -0.3));
     break;
   case 'k':
     sceneModelMatrix = Mult(sceneModelMatrix,ArbRotate(axis, -0.3));
     break;

   case 'p':
     if(isMoving == 1){
        isMoving = 0;
        printf("%s\n", "ismoving is 0");
        }
     else if(isMoving == 0){
        printf("%s\n", "ismoving is 1");
        isMoving = 1;
        }

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
    //glutKeyboardFunc(zprKey);

    glutMainLoop();
    exit(0);
}

