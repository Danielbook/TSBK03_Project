
// Converted to MicroGlut and VectorUtils3 2013.
// MicroGLut currently exists for Linux and Mac OS X, and a beta for Windows.
// You will also need GLEW for Windows. You may sometimes need to work around
// differences, e.g. additions in MicroGlut that don't exist in FreeGlut.

// 2015: 

// Linux: gcc lab0.c ../common/*.c ../common/Linux/MicroGlut.c -lGL -o lab0 -I../common -I../common/Linux

// Mac: gcc lab0.c ../common/*.c ../common/Mac/MicroGlut.m -o lab0 -framework OpenGL -framework Cocoa -I../common/Mac -I../common

#ifdef __APPLE__

#include <OpenGL/gl3.h>
#include "MicroGlut.h"
#include "noise1234.h"
//uses framework Cocoa
#else
#include <GL/gl.h>
#include "MicroGlut.h"
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
//#include "zpr.h"
#include "LoadTGA.h"

//constants
const int initWidth = 512, initHeight = 512;

// Model-to-world matrix
// Modify this matrix.
// See below for how it is applied to your model.

// World-to-view matrix. Usually set by lookAt() or similar.
mat4 viewMatrix;
// Projection matrix, set by a call to perspective().
mat4 projectionMatrix;

// Globals
// * Model(s)
Model *planet;
// * Reference(s) to shader program(s)
GLuint phongShader, planetShader;
// * Texture(s)
GLuint texture;

void init(void) {
    dumpInfo();

    // GL inits
    glClearColor(0.2, 0.2, 0.5, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    projectionMatrix = frustum(-0.5, 0.5, -0.5, 0.5, 1.0, 300.0);

    // Load and compile shader
    phongShader = loadShaders("../phong.vert", "../phong.frag");
    planetShader = loadShaders("../shaders/planet.vert", "../shaders/planet.frag");

    // Upload geometry to the GPU:
    planet = LoadModelPlus("../assets/sphere.obj"); // Sphere

    // Important! The shader we upload to must be active!
    glUseProgram(planetShader);
    glUniformMatrix4fv(glGetUniformLocation(phongShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

    printError("init arrays");
}

GLfloat a = 0.0;

void display(void) {
    printError("pre display");

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 worldToView, m; // m1, m2, m, tr, scale;

    worldToView = lookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);

    a += 0.03;

    mat4 sunPos, planetPos, moonPos, planetRotPos, moonRotPos;
    float xs = 0.0;
    float ys = 0.0;
    float zs = 0.0;
    float b = -a * 2;
    float c = a * 5;
    float d = a * 15;
    float rp = 2;
    float rm = 0.5;

    // Calculate matrices in matrix sequences that impose the dependencies.
    mat4 CAM = lookAt(0, 4, 4, 0, 0, 0, 0, 1, 0);
    sunPos = Mult(CAM, T(xs, ys, zs));
    planetPos = Mult(sunPos, Mult(Ry(a), T(rp, 0, 0))); // Planet position
    moonPos = Mult(planetPos, Mult(Ry(c), T(rm, 0, 0))); // Moon position
    planetRotPos = Mult(planetPos, Mult(Ry(b), S(0.3, 0.3, 0.3))); // Planet pos + rotation
    moonRotPos = Mult(moonPos, Mult(Ry(d), S(0.2, 0.2, 0.2))); // Moon pos + rotation

    glUseProgram(phongShader);
    glUniformMatrix4fv(glGetUniformLocation(phongShader, "modelviewMatrix"), 1, GL_TRUE, sunPos.m);
    DrawModel(planet, phongShader, "inPosition", "inNormal", NULL);

    glUseProgram(planetShader);
    glUniformMatrix4fv(glGetUniformLocation(planetShader, "modelviewMatrix"), 1, GL_TRUE, planetRotPos.m);
    DrawModel(planet, planetShader, "inPosition", "inNormal", NULL);

    printError("display");

    glutSwapBuffers();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitContextVersion(3, 2);
    glutCreateWindow("TSBK03 Project");
    glutDisplayFunc(display);
    glutRepeatingTimer(20);
    init();
    glutMainLoop();
    exit(0);
}

