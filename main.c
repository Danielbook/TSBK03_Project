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
#define W 1000
#define H 1000

// Projection matrix, set by a call to perspective().
mat4 projectionMatrix;
mat4 viewMatrix;

Point3D cam, point;

// * Model(s)
Model *sphere;
// * Reference(s) to shader program(s)
GLuint planetShader, sunShader, oceanShader;
// * Texture(s)
GLuint texture;

GLfloat a;

int frame = 0, time, timebase = 0, deltaTime = 0, startTime = 0;

typedef struct
{
    int triangle[3];
} triangleArray;

typedef struct
{
    float vertex[3];
} vertexArray;

// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new frame
void onTimer(int value)
{
  glutPostRedisplay();
  glutTimerFunc(5, &onTimer, value);
}

triangleArray *triangles = NULL;
vertexArray *vertices = NULL;


void init(void)
{
  dumpInfo();

  // GL inits
  glClearColor(0.0, 0.0, 0.0, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  projectionMatrix = frustum(-1, 1, -1, 1, 1.0, 1000.0);
  printError("GL inits");

  // Load and compile shaders
  planetShader = loadShadersG("../shaders/planet.vert", "../shaders/planet.frag", "../shaders/planet.geom");
  sunShader = loadShaders("../shaders/sun.vert", "../shaders/sun.frag");
  oceanShader = loadShaders("../shaders/ocean.vert", "../shaders/ocean.frag");

  printError("init shader");

  // Upload geometry to the GPU:
  sphere = LoadModelPlus("../assets/bestSphere.obj"); // Sphere

  int trianglesSize = sphere->numIndices / 3;

  triangles = malloc(sizeof(triangleArray) * trianglesSize);

  for (int i = 0; i < sphere->numIndices; i += 3) {
    triangles[i].triangle[0] = sphere->indexArray[i];
    triangles[i].triangle[1] = sphere->indexArray[i + 1];
    triangles[i].triangle[2] = sphere->indexArray[i + 2];

//    printf("Triangle: %i: ", i / 3 + 1);
//    printf("(%i, ", triangles[sphere->numIndices / 3].triangle[0]);
//    printf("%i, ", triangles[sphere->numIndices / 3].triangle[1]);
//    printf("%i)\n", triangles[sphere->numIndices / 3].triangle[2]);
  }

  vertices = malloc(sizeof(vertexArray) * sphere->numVertices);

  for (int i = 0; i < sphere->numVertices; i += 3) {
    vertices[i].vertex[0] = sphere->vertexArray[i];
    vertices[i + 1].vertex[1] = sphere->vertexArray[i + 1];
    vertices[i + 2].vertex[2] = sphere->vertexArray[i + 2];

//        printf("(%f, ", vertices[i].vertex[0]);
//        printf("%f, ", vertices[i+1].vertex[1]);
//        printf("%f)\n", vertices[i+2].vertex[2]);
  }

  for(int i = 0; i < 1; i++) {
    for(int j = 0; j < trianglesSize; j++) {
      // Edge 1
      if(triangles[i].triangle[0] == triangles[j].triangle[0] && triangles[i].triangle[1] == triangles[j].triangle[1] ||
         triangles[i].triangle[0] == triangles[j].triangle[1] && triangles[i].triangle[1] == triangles[j].triangle[2] ||
         triangles[i].triangle[0] == triangles[j].triangle[2] && triangles[i].triangle[1] == triangles[j].triangle[0]) {

        printf("Case 1!\n");
        printf("Triangle %i: ", i);
        printf("{%i, ", triangles[i].triangle[0]);
        printf("%i, ", triangles[i].triangle[1]);
        printf("%i}\n",triangles[i].triangle[2]);

        printf("Triangle %i: ", j);
        printf("{%i, ", triangles[j].triangle[0]);
        printf("%i, ", triangles[j].triangle[1]);
        printf("%i}\n",triangles[j].triangle[2]);

        printf("Unique vertice: %i", triangles[j].triangle[0]);
        printf("\n");
      }
      // Edge 2
      if(triangles[i].triangle[1] == triangles[j].triangle[1] && triangles[i].triangle[2] == triangles[j].triangle[2] ||
         triangles[i].triangle[1] == triangles[j].triangle[2] && triangles[i].triangle[2] == triangles[j].triangle[0] ||
         triangles[i].triangle[1] == triangles[j].triangle[0] && triangles[i].triangle[2] == triangles[j].triangle[1]) {

        printf("Case 2!\n");
        printf("Triangle %i: ", i);
        printf("{%i, ", triangles[i].triangle[0]);
        printf("%i, ", triangles[i].triangle[1]);
        printf("%i}\n",triangles[i].triangle[2]);

        printf("Triangle %i: ", j);
        printf("{%i, ", triangles[j].triangle[0]);
        printf("%i, ", triangles[j].triangle[1]);
        printf("%i}\n",triangles[j].triangle[2]);

        printf("Unique vertice: %i", triangles[j].triangle[1]);
        printf("\n");
      }
      // Edge 3
      if(triangles[i].triangle[2] == triangles[j].triangle[2] && triangles[i].triangle[0] == triangles[j].triangle[0] ||
         triangles[i].triangle[2] == triangles[j].triangle[1] && triangles[i].triangle[0] == triangles[j].triangle[2] ||
         triangles[i].triangle[2] == triangles[j].triangle[0] && triangles[i].triangle[0] == triangles[j].triangle[1]) {

        printf("Case 3!\n");
        printf("Triangle %i: ", i);
        printf("{%i, ", triangles[i].triangle[0]);
        printf("%i, ", triangles[i].triangle[1]);
        printf("%i}\n",triangles[i].triangle[2]);

        printf("Triangle %i: ", j);
        printf("{%i, ", triangles[j].triangle[0]);
        printf("%i, ", triangles[j].triangle[1]);
        printf("%i}\n",triangles[j].triangle[2]);

        printf("Unique vertice: %i", triangles[j].triangle[2]);
        printf("\n");
      }
    }
  }


  printError("load models");

  glutTimerFunc(5, &onTimer, 0);

  cam = SetVector(0, 300, 100);
  point = SetVector(0, 1, 0);

  zprInit(&viewMatrix, cam, point);
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

//  frame++;
//
//    // Print FPS
//    if (time - timebase > 1000) {
//        printf("FPS: %4.2f\n", frame * 1000.0 / (time - timebase));
//        timebase = time;
//        frame = 0;
//    }

  mat4 sunPos, planetPos, planetRotPos, planetOcean;

  float b = -a * 2;
  float c = a * 5;
  float d = a * 15;
  float rm = 0.5;

  // Calculate matrices in matrix sequences that impose the dependencies.
//  viewMatrix = lookAt(0, 2, 4,  // Camera is set in in World Space
//                      0, 0, 0,  // and looks at the origin
//                      0, 1, 0); // Head is up
  sunPos = Mult(viewMatrix, T(0, 0, 0));
  planetPos = Mult(sunPos, Mult(Ry(0), T(200, 0, 0))); // Planet position
  planetRotPos = Mult(planetPos, Mult(Ry(0), S(0.4, 0.4, 0.4))); // Planet pos + rotation
  planetOcean = Mult(planetRotPos, S(0.97, 0.97, 0.97)); // Planet pos + rotation

//  printMat4(planetRotPos);

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // SUN
  const float sunAmp = 0.1;
  const float sunFreq = 80;
  mat3 sunNormalMatrix = InverseTranspose(sunPos);

  glUseProgram(sunShader);
  glUniform1f(glGetUniformLocation(sunShader, "amplitude"), sunAmp);
  glUniform1f(glGetUniformLocation(sunShader, "frequency"), sunFreq);
  glUniform1f(glGetUniformLocation(sunShader, "time"), time);
  glUniformMatrix4fv(glGetUniformLocation(sunShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(sunShader, "modelViewMatrix"), 1, GL_TRUE, sunPos.m);
  glUniformMatrix4fv(glGetUniformLocation(sunShader, "normalMatrix"), 1, GL_TRUE, sunNormalMatrix.m);

  DrawModel(sphere, sunShader, "inPosition", "inNormal", NULL);

  // PLANET
  const float mountAmp = 20;
  const float mountFreq = 0.03;
  const float avgTemp = 7.0;

  vec4 lightPosition = {0.0, 0.0, 0.0, 1.0};
  vec3 surfaceColor = {0.0, 0.4, 0.1};
  vec3 snowColor = {0.8, 0.9, 1.0};
  vec3 sandColor = {0.95, 0.67, 0.26};

  mat3 planetNormalMatrix = InverseTranspose(planetRotPos);

  glUseProgram(planetShader);
  glUniform1f(glGetUniformLocation(planetShader, "amplitude"), mountAmp);
  glUniform1f(glGetUniformLocation(planetShader, "frequency"), mountFreq);
  glUniform1f(glGetUniformLocation(planetShader, "avgTemp"), avgTemp);
  glUniform4f(glGetUniformLocation(planetShader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z,
              lightPosition.w);
  glUniform3f(glGetUniformLocation(planetShader, "surfaceColor"), surfaceColor.x, surfaceColor.y, surfaceColor.z);
  glUniform3f(glGetUniformLocation(planetShader, "snowColor"), snowColor.x, snowColor.y, snowColor.z);
  glUniform3f(glGetUniformLocation(planetShader, "sandColor"), sandColor.x, sandColor.y, sandColor.z);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);
  glUniformMatrix3fv(glGetUniformLocation(planetShader, "normalMatrix"), 1, GL_TRUE, planetNormalMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(planetShader, "modelViewMatrix"), 1, GL_TRUE, planetRotPos.m);


//  DrawModel(sphere, planetShader, "inPosition", "inNormal", NULL);

  /*
   * Stencil testing
   * http://ogldev.atspace.co.uk/www/tutorial40/tutorial40.html
  */

  // Clear the screen to white
//  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  glStencilFunc(GL_ALWAYS, 0, 0xFF); // init to 0, camera is not in shadowed area

  //Increment stencil when entering object
  glCullFace(GL_FRONT);
  glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

  //Decrement stencil when exiting object
  glCullFace(GL_BACK);
  glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

  DrawModel(sphere, planetShader, "inPosition", "inNormal", NULL);

  glDisable(GL_STENCIL_TEST);

  // Ocean
  vec3 oceanColor = {0, 11 / 255, 255 / 255};

  glUseProgram(oceanShader);
  glUniform1f(glGetUniformLocation(oceanShader, "time"), time);
  glUniform4f(glGetUniformLocation(oceanShader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z,
              lightPosition.w);
  glUniform1f(glGetUniformLocation(oceanShader, "avgTemp"), avgTemp);
  glUniform3f(glGetUniformLocation(oceanShader, "oceanColor"), oceanColor.x, oceanColor.y, oceanColor.z);
  glUniformMatrix4fv(glGetUniformLocation(oceanShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(oceanShader, "modelViewMatrix"), 1, GL_TRUE, planetOcean.m);
  DrawModel(sphere, oceanShader, "inPosition", "inNormal", NULL);

  printError("display");

  glutSwapBuffers();
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
  glutInitWindowSize(W, H);

  glutInitContextVersion(3, 2);
  glutCreateWindow("TSBK03 Project");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);

  init();
  glutMainLoop();
  exit(0);
}

