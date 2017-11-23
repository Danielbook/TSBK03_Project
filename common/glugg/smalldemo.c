
// Four BŽzier patches, with an adjustable "joystick" control for the 3x3 center control points.
// By Ingemar 2016
// Uses my "GLUGG, OpenGL Utility for Geometry Generation" for generating patches.
// Current flaw: Should dispose of old buffers when making new ones! Or reuse the old.


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
// uses framework Cocoa
#else
	#include "MicroGlut.h"
//	#include <GL/glut.h>
#endif
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "glugg.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

/*
 0  1  2  3  4  5  6
 7  8  9 10 11 12 13
14 15 16 17 18 19 20
21 22 23 24 25 26 27
28 29 30 31 32 33 34
35 36 37 38 39 40 41
42 43 44 45 46 47 48
*/

int patchIndices[][16] = 
{
	{ 0, 1, 2, 3,  7, 8, 9,10, 14,15,16,17, 21,22,23,24},
	{ 3, 4, 5, 6, 10,11,12,13, 17,18,19,20, 24,25,26,27},
	{21,22,23,24, 28,29,30,31, 35,36,37,38, 42,43,44,45},
	{24,25,26,27, 31,32,33,34, 38,39,40,41, 45,46,47,48},
};

vec3 patchVertices[] =
{
	{-0.9, 0.0, -0.9},  // 0
	{-0.6, 0.0, -0.9},  // 1
	{-0.3, 0.0, -0.9},  // 2
	{ 0.0, 0.0, -0.9},  // 3
	{ 0.3, 0.0, -0.9},  // 4	
	{ 0.6, 0.0, -0.9},  // 5
	{ 0.9, 0.0, -0.9},  // 6

	{-0.9, 0.0, -0.6},  // 7
	{-0.6, 0.0, -0.6},  // 8
	{-0.3, 0.0, -0.6},  // 9
	{ 0.0, 0.0, -0.6},  // 10
	{ 0.3, 0.0, -0.6},  // 11	
	{ 0.6, 0.0, -0.6},  // 12
	{ 0.9, 0.0, -0.6},  // 13

	{-0.9, 0.0, -0.3},  // 14
	{-0.6, 0.0, -0.3},  // 15
	{-0.3,-1.0, -0.3},  // 16
	{ 0.0,-1.0, -0.3},  // 17
	{ 0.3,-1.0, -0.3},  // 18	
	{ 0.6, 0.0, -0.3},  // 19
	{ 0.9, 0.0, -0.3},  // 20

	{-0.9, 0.0,  0.0},  // 21
	{-0.6, 0.0,  0.0},  // 22
	{-0.3,-1.0,  0.0},  // 23
	{ 0.0,-1.0,  0.0},  // 24
	{ 0.3,-1.0,  0.0},  // 25	
	{ 0.6, 0.0,  0.0},  // 26
	{ 0.9, 0.0,  0.0},  // 27

	{-0.9, 0.0,  0.3},  // 28
	{-0.6, 0.0,  0.3},  // 29
	{-0.3,-1.0,  0.3},  // 30
	{ 0.0,-1.0,  0.3},  // 31
	{ 0.3,-1.0,  0.3},  // 32	
	{ 0.6, 0.0,  0.3},  // 33
	{ 0.9, 0.0,  0.3},  // 34

	{-0.9, 0.0,  0.6},  // 35
	{-0.6, 0.0,  0.6},  // 36
	{-0.3, 0.0,  0.6},  // 37
	{ 0.0, 0.0,  0.6},  // 38
	{ 0.3, 0.0,  0.6},  // 39	
	{ 0.6, 0.0,  0.6},  // 40
	{ 0.9, 0.0,  0.6},  // 41

	{-0.9, 0.0,  0.9},  // 42
	{-0.6, 0.0,  0.9},  // 43
	{-0.3, 0.0,  0.9},  // 44
	{ 0.0, 0.0,  0.9},  // 45
	{ 0.3, 0.0,  0.9},  // 46	
	{ 0.6, 0.0,  0.9},  // 47
	{ 0.9, 0.0,  0.9},  // 48
};

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLfloat angle = 0.0;
static float rot = 0.0;
int wireframe = 0;

// The teaset objects and lengths
GLuint theVao;
GLsizei triangles;

GLuint program; // Shader
mat4 projectionMatrix;

static GLfloat color[4] = { 0.8, 0.1, 0.0, 1.0 }; // Red

static void
draw(void)
{
	printError("pre display");

	angle += rot;
	mat4 m;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m = Mult(lookAt(5,0,5, 0,0.5,0, 0,1,0), Mult(Rx(0.4), Mult(Ry(angle), Rx(3))));
		
		glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"), 1, GL_TRUE, m.m);
		glUniform4fv(glGetUniformLocation(program, "color"), 1, color);
		glBindVertexArray(theVao);	// Select VAO
		if (wireframe)
			glDrawElements(GL_LINES, triangles, GL_UNSIGNED_INT, NULL);
		else
			glDrawElements(GL_TRIANGLES, triangles, GL_UNSIGNED_INT, NULL);

	glutSwapBuffers();
	
	printError("display");
}


/* new window size or exposure */
static void
reshape(int width, int height)
{
	GLfloat h = (GLfloat) height / (GLfloat) width;

	glViewport(0, 0, (GLint) width, (GLint) height);
//	projectionMatrix = frustum(-1.0, 1.0, -h, h, 5.0, 60.0);
//	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

static GLfloat pos[4] = { 5.0, 5.0, 10.0, 0.0 };

// Reload mesh!
void UpdatePatches()
{
	// Should dispose of old data here
	theVao = gluggBuildBezierPatchModel(patchVertices, patchIndices, 0, 4, &triangles, program, 0.05);
}

static void
init(void)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(1,1,1,1);
	
	program = loadShaders("tea.vert", "tea.frag");
	glUniform4fv(glGetUniformLocation(program, "pos"), 1, pos);
	projectionMatrix = frustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	printf("SHADERS LOADED\n");
	
	/* make the patch */
	theVao = gluggBuildBezierPatchModel(patchVertices, patchIndices, 0, 4, &triangles, program, 0.05);
	UpdatePatches();
	projectionMatrix = frustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0);
}

// Very cheap and simple way to move control points, just offsetting the Y value on either side.
// A real program would work from normal vectors. 

void keys(unsigned char c, int x, int y)
{
	switch (c)
	{
		case 'a': 
			patchVertices[16].y += 0.1;
			patchVertices[17].y += 0.1;
			patchVertices[18].y += 0.1;
			patchVertices[30].y -= 0.1;
			patchVertices[31].y -= 0.1;
			patchVertices[32].y -= 0.1;
			break;
		case 'd':
			patchVertices[16].y -= 0.1;
			patchVertices[17].y -= 0.1;
			patchVertices[18].y -= 0.1;
			patchVertices[30].y += 0.1;
			patchVertices[31].y += 0.1;
			patchVertices[32].y += 0.1;
			break;
		case 'w':
			patchVertices[16].y -= 0.1;
			patchVertices[23].y -= 0.1;
			patchVertices[30].y -= 0.1;
			patchVertices[18].y += 0.1;
			patchVertices[25].y += 0.1;
			patchVertices[32].y += 0.1;
			break;
		case 's':
			patchVertices[16].y += 0.1;
			patchVertices[23].y += 0.1;
			patchVertices[30].y += 0.1;
			patchVertices[18].y -= 0.1;
			patchVertices[25].y -= 0.1;
			patchVertices[32].y -= 0.1;
			break;
		case '-':
			patchVertices[16].y += 0.1;
			patchVertices[17].y += 0.1;
			patchVertices[18].y += 0.1;
			patchVertices[23].y += 0.1;
			patchVertices[24].y += 0.1;
			patchVertices[25].y += 0.1;
			patchVertices[30].y += 0.1;
			patchVertices[31].y += 0.1;
			patchVertices[32].y += 0.1;
			break;
		case '+':
			patchVertices[16].y -= 0.1;
			patchVertices[17].y -= 0.1;
			patchVertices[18].y -= 0.1;
			patchVertices[23].y -= 0.1;
			patchVertices[24].y -= 0.1;
			patchVertices[25].y -= 0.1;
			patchVertices[30].y -= 0.1;
			patchVertices[31].y -= 0.1;
			patchVertices[32].y -= 0.1;
			break;
		case ' ':
			rot = 0.05 - rot; break;
		case 'v':
			wireframe = 1 - wireframe; break;
		case 0x1b: exit(0);
	}
	UpdatePatches();
}


int
main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("Small CPU-tesselation, 4 patches");
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(keys);
	glutRepeatingTimer(20);

	init();

	glutMainLoop();

	return 0;
}
