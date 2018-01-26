#ifdef __APPLE__

#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <loadobj.h>
#include "MicroGlut.h"
//uses framework Cocoa
#else
#include <GL/gl.h>
#include "MicroGlut.h"
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "zpr.h"
#include "LoadTGA.h"

// initial width and heights
#define RENDER_WIDTH 1000
#define RENDER_HEIGHT 1000

// Projection matrix, set by a call to perspective().
mat4 projectionMatrix;
mat4 viewMatrix, textureMatrix;

Point3D cam, point;

// * Model(s)
Model *sphere;
// * Reference(s) to shader program(s)
GLuint planetShaderId, sunShaderId, oceanShaderId, projTexShaderId;
// Use to activate/disable projTexShader
GLuint projTexMapUniform;

FBOstruct *fbo;

//Light mouvement circle radius
float light_mvnt = 40.0f; // At 30 we get edge artifacts

//Light position
Point3D p_light = {40,20,0};

//Light lookAt
Point3D l_light = {0,3,-10};

//Camera position
Point3D p_camera = {0, 300, 100};

//Camera lookAt
Point3D l_camera = {0, 1, 0};


// * Texture(s)
GLuint texture;

GLuint data[3];

GLfloat a;


int frame = 0, time, timebase = 0, deltaTime = 0, startTime = 0, nVertices = 0;

typedef struct
{
    int triangle[3];
} triangleArray;

typedef struct
{
    float vertex[3];
} vertexArray;

float vertArray;

// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new frame
void onTimer(int value)
{
  glutPostRedisplay();
  glutTimerFunc(5, &onTimer, value);
}


void loadShadowShaders()
{
  // Shadow map
  projTexShaderId = loadShaders("../shaders/shadowmap.vert", "../shaders/shadowmap.frag");
  projTexMapUniform = glGetUniformLocation(projTexShaderId, "textureUnit");

  // Load and compile shaders
  planetShaderId = loadShaders("../shaders/planet.vert", "../shaders/planet.frag");
  sunShaderId = loadShaders("../shaders/sun.vert", "../shaders/sun.frag");
  oceanShaderId = loadShaders("../shaders/ocean.vert", "../shaders/ocean.frag");

  printError("init shader");
}

void init(void)
{
  dumpInfo();

  // GL inits
  glClearColor(0.0, 0.0, 0.0, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  projectionMatrix = frustum(-1, 1, -1, 1, 1.0, 1000.0);
  printError("GL inits");

  // Upload geometry to the GPU:
  sphere = LoadModelPlus("../assets/newSphere.obj"); // Sphere

  printError("load models");

  glutTimerFunc(5, &onTimer, 0);

  cam = SetVector(0, 300, 100);
  point = SetVector(0, 1, 0);

  zprInit(&viewMatrix, p_camera, l_camera);
}

void updatePositions() {
  p_light.x = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME)/1000.0);
  p_light.z = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME)/1000.0);
}

mat4 sunPos;

void drawObjects(GLuint shader)
{
  mat4 planetPos, planetRotPos, planetOcean;

  time = glutGet(GLUT_ELAPSED_TIME);

  deltaTime = time - startTime;
  startTime = time;

  sunPos = Mult(viewMatrix, T(0, 0, 0));
  planetPos = Mult(sunPos, Mult(Ry(0), T(200, 0, 0))); // Planet position
  planetRotPos = Mult(planetPos, Mult(Ry(0.005*time), S(0.4, 0.4, 0.4))); // Planet pos + rotation
  planetOcean = Mult(planetRotPos, S(0.97, 0.97, 0.97)); // Planet pos + rotation

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);



  // SUN
  const float sunAmp = 0.1;
  const float sunFreq = 80;
  mat3 sunNormalMatrix = InverseTranspose(sunPos);

  glUseProgram(sunShaderId);
  glUniform1f(glGetUniformLocation(sunShaderId, "amplitude"), sunAmp);
  glUniform1f(glGetUniformLocation(sunShaderId, "frequency"), sunFreq);
  glUniform1f(glGetUniformLocation(sunShaderId, "time"), time);
  glUniformMatrix4fv(glGetUniformLocation(sunShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(sunShaderId, "modelViewMatrix"), 1, GL_TRUE, sunPos.m);
  glUniformMatrix4fv(glGetUniformLocation(sunShaderId, "normalMatrix"), 1, GL_TRUE, sunNormalMatrix.m);

  DrawModel(sphere, sunShaderId, "inPosition", "inNormal", NULL);

  // PLANET
  const float mountAmp = 20;
  const float mountFreq = 0.03;
  const float avgTemp = 7.0;

  vec3 surfaceColor = {0.0, 0.4, 0.1};
  vec3 snowColor = {0.8, 0.9, 1.0};
  vec3 sandColor = {0.95, 0.67, 0.26};

  mat3 planetNormalMatrix = InverseTranspose(planetRotPos);

  glUseProgram(planetShaderId);
  glUniform1f(glGetUniformLocation(planetShaderId, "amplitude"), mountAmp);
  glUniform1f(glGetUniformLocation(planetShaderId, "frequency"), mountFreq);
  glUniform1f(glGetUniformLocation(planetShaderId, "avgTemp"), avgTemp);
  glUniform3f(glGetUniformLocation(planetShaderId, "lightPosition"), p_light.x, p_light.y, p_light.z);
  glUniform3f(glGetUniformLocation(planetShaderId, "surfaceColor"), surfaceColor.x, surfaceColor.y, surfaceColor.z);
  glUniform3f(glGetUniformLocation(planetShaderId, "snowColor"), snowColor.x, snowColor.y, snowColor.z);
  glUniform3f(glGetUniformLocation(planetShaderId, "sandColor"), sandColor.x, sandColor.y, sandColor.z);
  glUniformMatrix4fv(glGetUniformLocation(planetShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(planetShaderId, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
  glUniformMatrix3fv(glGetUniformLocation(planetShaderId, "normalMatrix"), 1, GL_TRUE, planetNormalMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(planetShaderId, "modelViewMatrix"), 1, GL_TRUE, planetRotPos.m);

  // Ocean
  vec3 oceanColor = {0, 11 / 255, 255 / 255};

  glUseProgram(oceanShaderId);
  glUniform1f(glGetUniformLocation(oceanShaderId, "time"), time);
  glUniform3f(glGetUniformLocation(oceanShaderId, "lightPosition"), p_light.x, p_light.y, p_light.z);
  glUniform1f(glGetUniformLocation(oceanShaderId, "avgTemp"), avgTemp);
  glUniform3f(glGetUniformLocation(oceanShaderId, "oceanColor"), oceanColor.x, oceanColor.y, oceanColor.z);
  glUniformMatrix4fv(glGetUniformLocation(oceanShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(oceanShaderId, "modelViewMatrix"), 1, GL_TRUE, planetOcean.m);
  DrawModel(sphere, oceanShaderId, "inPosition", "inNormal", NULL);
}


void setTextureMatrix(void)
{
  mat4 scaleBiasMatrix;

  IdentityMatrix(textureMatrix);

// Scale and bias transform, moving from unit cube [-1,1] to [0,1]
  scaleBiasMatrix = Mult(T(0.5, 0.5, 0.0), S(0.5, 0.5, 1.0));
  textureMatrix = Mult(Mult(scaleBiasMatrix, projectionMatrix), viewMatrix);
  // Multiply modelview and transformation matrices
}

void renderScene(void)
{
  printError("Render Scene");

  updatePositions();

  setTextureMatrix();

  viewMatrix = lookAt(p_light.x, p_light.y, p_light.z,
                      l_light.x, l_light.y, l_light.z, 0,1,0);

  // 1. Render scene to FBO

  useFBO(fbo, NULL, NULL);
  glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE); // Depth only
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Using the simple shader
  glUseProgram(projTexShaderId);
  glUniform1i(projTexMapUniform, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  drawObjects(planetShaderId);

  //2. Render from camera.
  // Now rendering from the camera POV

  useFBO(NULL, fbo, NULL);

  glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

  //Enabling color write (previously disabled for light POV z-buffer rendering)
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  // Clear previous frame values
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Using the projTex shader
  glUseProgram(projTexShaderId);
  glUniform1i(projTexMapUniform, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, fbo->depth);

  // Setup the modelview from the camera
  viewMatrix = lookAt(p_camera.x, p_camera.y, p_camera.z,
                  l_camera.x, l_camera.y, l_camera.z, 0, 1, 0);

  glCullFace(GL_BACK);
  drawObjects(projTexShaderId);

  glutSwapBuffers();

//  // clear the screen
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//
//  time = glutGet(GLUT_ELAPSED_TIME);
//
//  deltaTime = time - startTime;
//  startTime = time;

//  frame++;
//
//    // Print FPS
//    if (time - timebase > 1000) {
//        printf("FPS: %4.2f\n", frame * 1000.0 / (time - timebase));
//        timebase = time;
//        frame = 0;
//    }




//  DrawModel(sphere, planetShaderId, "inPosition", "inNormal", NULL);

  /*
   * Stencil testing
   * http://ogldev.atspace.co.uk/www/tutorial40/tutorial40.html
  */

  // Clear the screen to white
//  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

//  glEnable(GL_DEPTH_TEST);
//  glEnable(GL_STENCIL_TEST);
//
//  glStencilFunc(GL_ALWAYS, 0, 0xFF); // init to 0, camera is not in shadowed area
//
//  //Increment stencil when entering object
//  glCullFace(GL_FRONT);
//  glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
//
//  //Decrement stencil when exiting object
//  glCullFace(GL_BACK);
//  glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
//
//  DrawModel(sphere, planetShaderId, "inPosition", "inNormal", NULL);
//
//  glDisable(GL_STENCIL_TEST);

//  printError("display");
//
//  glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
  glViewport(0, 0, w, h);
  GLfloat ratio = (GLfloat) w / (GLfloat) h;
  projectionMatrix = perspective(90, ratio, 1.0, 1000);

}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
  glutInitWindowSize(RENDER_WIDTH, RENDER_HEIGHT);
  glutInitContextVersion(3, 2);
  glutCreateWindow("TSBK03 Project");

  loadShadowShaders();

  fbo = initFBO2(RENDER_WIDTH, RENDER_HEIGHT, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glClearColor(0, 0, 0, 1.0f);
  glEnable(GL_CULL_FACE);

  glutDisplayFunc(renderScene);
  glutReshapeFunc(reshape);

  init();
  glutMainLoop();
  exit(0);
}

