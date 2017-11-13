#ifdef __APPLE__

#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
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
#define W 500
#define H 500

// World-to-view matrix. Usually set by lookAt() or similar.
mat4 viewMatrix;

// Projection matrix, set by a call to perspective().
mat4 projectionMatrix;

// Globals
Point3D cam, point;
// * Model(s)
Model *sphere;
// * Reference(s) to shader program(s)
GLuint planetShader, sunShader;
// * Texture(s)
GLuint texture;

GLfloat a;

int frame = 0, time, timebase = 0, deltaTime = 0, startTime = 0;

void init(void)
{
  dumpInfo();

  // GL inits
  glClearColor(0.0, 0.0, 0.0, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  projectionMatrix = frustum(-0.5, 0.5, -0.5, 0.5, 1.0, 300.0);
  printError("GL inits");

  // Load and compile shaders
//    noiseShader = loadShader("../shaders/noise/classicnoise3D.glsl");
  planetShader = loadShadersG("../shaders/planet.vert", "../shaders/planet.frag", "../shaders/planet.geom");
  sunShader = loadShaders("../shaders/sun.vert", "../shaders/sun.frag");

  printError("init shader");

  // Upload geometry to the GPU:
  sphere = LoadModelPlus("../assets/sphere64.obj"); // Sphere

  printError("load models");

  cam = SetVector(3, 2, 3);
  point = SetVector(0, 0, 0);
}

void display(void)
{
  printError("pre display");

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  a += 0.01;

  time = glutGet(GLUT_ELAPSED_TIME);

  deltaTime = time - startTime;
  startTime = time;

  //Varying time
  frame++;

//    // Print FPS
//    if (time - timebase > 1000) {
//        printf("FPS: %4.2f\n", frame * 1000.0 / (time - timebase));
//        timebase = time;
//        frame = 0;
//    }

  mat4 sunPos, planetPos, moonPos, planetRotPos, moonRotPos;

  mat4 A, B, C, D, E;
  float xs = 0.0;
  float ys = 0.0;
  float zs = 0.0;
  float b = -a * 2;
  float c = a * 5;
  float d = a * 15;
  float rp = 2;
  float rm = 0.5;

  // Calculate matrices in matrix sequences that impose the dependencies.
  mat4 CAM = lookAt(0, 2, 4, 0, 0, 0, 0, 1, 0);
  sunPos = Mult(CAM, T(xs, ys, zs));
  planetPos = Mult(sunPos, Mult(Ry(0), T(rp, 0, 0))); // Planet position
  planetRotPos = Mult(planetPos, Mult(Ry(c), S(0.4, 0.4, 0.4))); // Planet pos + rotation

//  printMat4(sunPos);
//  printf("|%f, ", sunPos.m[4]);
//  printf("%f, ", sunPos.m[7]);
//  printf("%f|\n", sunPos.m[11]);
//  printf("%s\n","-------------------------------");


  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // SUN
  const float sunAmp = 0.1;
  const float sunFreq = 80;

  glUseProgram(sunShader);
  glUniform1f(glGetUniformLocation(sunShader, "amplitude"), sunAmp);
  glUniform1f(glGetUniformLocation(sunShader, "frequency"), sunFreq);
  glUniform1f(glGetUniformLocation(sunShader, "time"), time);
  glUniformMatrix4fv(glGetUniformLocation(sunShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(sunShader, "modelviewMatrix"), 1, GL_TRUE, sunPos.m);

  DrawModel(sphere, sunShader, "inPosition", "inNormal", NULL);

//  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//  GLfloat mat_shininess[] = { 50.0 };
//  GLfloat light_position[] = { 1.0, 0.0, 0.0, 0.0 };
//  glClearColor (0.0, 0.0, 0.0, 0.0);
//  glShadeModel (GL_SMOOTH);
//
//  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//
//  glEnable(GL_LIGHTING);
//  glEnable(GL_LIGHT0);

  // PLANET
  const float mountAmp = 0.5;
  const float mountFreq = 5;
  const float avgTemp = 7.0;
  vec3 surfaceColor = {0.0, 0.4, 0.1};
  vec3 snowColor = {0.8, 0.9, 1.0};
  vec3 sandColor = {0.95, 0.67, 0.26};
  vec3 lightPos = {0.0, 0.0, 0};

  glUseProgram(planetShader);
  glUniform1f(glGetUniformLocation(planetShader, "amplitude"), mountAmp);
  glUniform1f(glGetUniformLocation(planetShader, "frequency"), mountFreq);
  glUniform1f(glGetUniformLocation(planetShader, "avgTemp"), avgTemp);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "sunPos"), 1, GL_TRUE, sunPos.m);
  glUniform3f(glGetUniformLocation(planetShader, "surfaceColor"), surfaceColor.x, surfaceColor.y, surfaceColor.z);
  glUniform3f(glGetUniformLocation(planetShader, "snowColor"), snowColor.x, snowColor.y, snowColor.z);
  glUniform3f(glGetUniformLocation(planetShader, "sandColor"), sandColor.x, sandColor.y, sandColor.z);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "viewMatrix"), 1, GL_TRUE, CAM.m);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "modelviewMatrix"), 1, GL_TRUE, planetRotPos.m);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

  DrawModel(sphere, planetShader, "inPosition", "inNormal", NULL);

  printError("display");

  glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
  glViewport(0, 0, w, h);
  GLfloat ratio = (GLfloat) w / (GLfloat) h;
  projectionMatrix = perspective(70, ratio, 0.2, 1000.0);
  glUniformMatrix4fv(glGetUniformLocation(sunShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new frame
void onTimer(int value)
{
  glutPostRedisplay();
  glutTimerFunc(5, &onTimer, value);
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(W, H);

  glutInitContextVersion(3, 2);
  glutCreateWindow("TSBK03 Project");
  glutDisplayFunc(display);

  glutTimerFunc(5, &onTimer, 0);
  glutReshapeFunc(reshape);

  init();
  zprInit(&viewMatrix, cam, point);
  glutMainLoop();
  exit(0);
}

