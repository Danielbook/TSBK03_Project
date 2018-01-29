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

#define TEX_UNIT 0

// Projection matrix, set by a call to perspective().
mat4 projectionMatrix;
mat4 viewMatrix, textureMatrix, modelViewMatrix;

Point3D cam, point;

// * Model(s)
Model *sphere;
// * Reference(s) to shader program(s)
GLuint planetShaderId, sunShaderId, oceanShaderId, moonShaderId, projTexShaderId;
// Use to activate/disable projTexShader
GLuint projTexMapUniform;

FBOstruct *fbo;

//Light mouvement circle radius
float light_mvnt = 40.0f; // At 30 we get edge artifacts

//Light position
Point3D p_light = {0, 0, 0};

//Light lookAt, Cube map here for point light???
Point3D l_light = {1, 0, 0};
//Point3D l_light = {0, 1, 0};
//Point3D l_light = {0, 0, 1};
//Point3D l_light = {-1, 0, 0};
//Point3D l_light = {0, -1, 0};
//Point3D l_light = {0, 0, -1};

//Camera position
Point3D p_camera = {0, 300, 100};

//Camera lookAt
Point3D l_camera = {0, 0, 0};

// * Texture(s)
GLuint texture;

GLfloat a;

int frame = 0, time, timebase = 0, deltaTime = 0, startTime = 0, nVertices = 0;

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
  moonShaderId = loadShaders("../shaders/moon.vert", "../shaders/moon.frag");

  printError("init shader");
}

GLfloat groundcolor[] = {0.3f, 0.3f, 0.3f, 1};
GLfloat ground[] = {
        -350, 0, -350,
        -350, 0, 350,
        350, 0, 350,
        350, 0, -350
};
GLuint groundIndices[] = {0, 1, 2, 0, 2, 3};

Model *groundModel;

void init(void)
{
  dumpInfo();

  // The shader must be loaded before this is called!
  if (projTexShaderId == 0)
    printf("Warning! Is the shader not loaded?\n");

  // GL inits
//  projectionMatrix = perspective(45, RENDER_WIDTH / RENDER_HEIGHT, 1.0, 4000);
  projectionMatrix = frustum(-1, 1, -1, 1, 1.0, 1000.0);
  printError("GL inits");

  // Upload geometry to the GPU:
  sphere = LoadModelPlus("../assets/newSphere.obj"); // Sphere

  groundModel = LoadDataToModel(
          ground,
          NULL,
          NULL,
          NULL,
          groundIndices,
          4,
          6);

  printError("load models");

  glutTimerFunc(5, &onTimer, 0);

  cam = SetVector(p_camera.x, p_camera.y, p_camera.z);
  point = SetVector(l_camera.x, l_camera.y, l_camera.z);

  zprInit(&viewMatrix, cam, point);
}

void updatePositions()
{
  p_light.x = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
  p_light.z = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
}

mat4 sunPos, planetPos;

void drawObjects(GLuint shader)
{
  mat4 planetRotPos, planetOcean, moonPos, moonRotPos;

  time = glutGet(GLUT_ELAPSED_TIME);

  deltaTime = time - startTime;
  startTime = time;

  sunPos = Mult(viewMatrix, T(0, 0, 0));

  planetPos = Mult(sunPos, Mult(Ry(0), T(200, 0, 0))); // Planet position
  planetRotPos = Mult(planetPos, Mult(Ry(0), S(0.4, 0.4, 0.4))); // Planet pos + rotation
  planetOcean = Mult(planetRotPos, S(0.97, 0.97, 0.97)); // Planet pos + rotation

  moonPos = Mult(planetPos, Mult(Ry(0), T(50, 0, 0))); // Moon rotation
  moonRotPos = Mult(moonPos, Mult(Ry(0), S(0.4, 0.4, 0.4))); // Planet pos + rotation

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

  mat4 planePos = Mult(planetPos, Mult(Rz(2), T(0, -100, 0)));

  // Ground
  glUniform1f(glGetUniformLocation(projTexShaderId, "shade"), 0.5); // Dark ground
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "modelViewMatrix"), 1, GL_TRUE, planePos.m);
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
  DrawModel(groundModel, projTexShaderId, "in_Position", NULL, NULL);

  glUniform1f(glGetUniformLocation(projTexShaderId, "shade"), 0.9); // Brighter objects

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
//  glUniformMatrix4fv(glGetUniformLocation(sunShaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);


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
  glUniformMatrix4fv(glGetUniformLocation(planetShaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);

  DrawModel(sphere, planetShaderId, "inPosition", "inNormal", NULL);

  // Ocean
//  vec3 oceanColor = {0, 11 / 255, 255 / 255};
//
//  glUseProgram(oceanShaderId);
//  glUniform1f(glGetUniformLocation(oceanShaderId, "time"), time);
//  glUniform3f(glGetUniformLocation(oceanShaderId, "lightPosition"), p_light.x, p_light.y, p_light.z);
//  glUniform1f(glGetUniformLocation(oceanShaderId, "avgTemp"), avgTemp);
//  glUniform3f(glGetUniformLocation(oceanShaderId, "oceanColor"), oceanColor.x, oceanColor.y, oceanColor.z);
//  glUniformMatrix4fv(glGetUniformLocation(oceanShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
//  glUniformMatrix4fv(glGetUniformLocation(oceanShaderId, "modelViewMatrix"), 1, GL_TRUE, planetOcean.m);
//  glUniformMatrix4fv(glGetUniformLocation(oceanShaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
//
//  DrawModel(sphere, oceanShaderId, "inPosition", "inNormal", NULL);

  // Moon
//  vec3 moonColor = {0, 11 / 255, 11 / 255};
//  const float moonMountFreq = 0.01;
//  const float moonMountAmp = 0.5;
//
//  glUseProgram(moonShaderId);
//  glUniform1f(glGetUniformLocation(moonShaderId, "time"), time);
//  glUniform1f(glGetUniformLocation(moonShaderId, "moonMountFreq"), moonMountFreq);
//  glUniform1f(glGetUniformLocation(moonShaderId, "moonMountAmp"), moonMountAmp);
//  glUniform3f(glGetUniformLocation(moonShaderId, "lightPosition"), p_light.x, p_light.y, p_light.z);
//  glUniform1f(glGetUniformLocation(moonShaderId, "avgTemp"), avgTemp);
//  glUniform3f(glGetUniformLocation(moonShaderId, "moonColor"), moonColor.x, moonColor.y, moonColor.z);
//  glUniformMatrix4fv(glGetUniformLocation(moonShaderId, "modelViewMatrix"), 1, GL_TRUE, moonRotPos.m);
//  glUniformMatrix4fv(glGetUniformLocation(moonShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
//  glUniformMatrix4fv(glGetUniformLocation(moonShaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
//
//  DrawModel(sphere, moonShaderId, "inPosition", "inNormal", NULL);
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

//  updatePositions();

//  projectionMatrix = perspective(45, RENDER_WIDTH/RENDER_HEIGHT, 10, 4000);

  // Setup the modelview from the light source
  viewMatrix = lookAt(p_light.x, p_light.y, p_light.z,
                      l_light.x, l_light.y, l_light.z, 0, 1, 0);

  setTextureMatrix();

  // 1. Render scene to FBO
  useFBO(fbo, NULL, NULL);
  glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE); // Depth only
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Using the simple shader
  glUseProgram(projTexShaderId);
  glUniform1i(projTexMapUniform, TEX_UNIT);
  glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
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
  glUniform1i(projTexMapUniform, TEX_UNIT);
  glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
  glBindTexture(GL_TEXTURE_2D, fbo->depth);

  // Setup the modelview from the camera
  viewMatrix = lookAt(p_camera.x, p_camera.y, p_camera.z,
                      l_camera.x, l_camera.y, l_camera.z, 0, 1, 0);

  updateCameraMatrix(NULL);

  glCullFace(GL_BACK);
  drawObjects(projTexShaderId);

  glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
  glViewport(0, 0, w, h);
  GLfloat ratio = (GLfloat) w / (GLfloat) h;
  projectionMatrix = perspective(90, ratio, 1.0, 4000);
}

void processNormalKeys(unsigned char key, int x, int y)
{
  // ESC key will close application
  if (key == 27)
    exit(0);

  // Press f and its fullscreen
  if (key == 102)
    glutFullScreen();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
  glutInitWindowSize(RENDER_WIDTH, RENDER_HEIGHT);
  glutInitContextVersion(3, 2);
  glutCreateWindow("TSBK03 Project");

  loadShadowShaders();
  init();

  fbo = initFBO2(RENDER_WIDTH, RENDER_HEIGHT, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glClearColor(1, 1, 1, 1.0f);
  glEnable(GL_CULL_FACE);

  glutDisplayFunc(renderScene);
//  glutReshapeFunc(reshape);

  glutKeyboardFunc(processNormalKeys);
  glutMainLoop();
  exit(0);
}

