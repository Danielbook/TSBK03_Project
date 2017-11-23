// Teapot 3 - the Utah teapot, cup, plate and spoon tesselated on the GPU.
// By Ingemar Ragnemalm 2013, using something as close to the original data as possible,
// but with some fixes for the most disturbing problems.
// Based on my "glutgears3" demo, which was based on the "glutgears" demo.
// You may note that the usual bug in a certain place (the knob) is fixed.

// Tesselation density
// This is nice: #define step 0.1
// This is crude: #define step 0.5
#define kStep 0.1

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
#include "teaset-data.h"
#include "glugg.h"
#include "saveobj.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLfloat angle = 0.0;

// The teaset objects and lengths
GLuint vao[12];
GLsizei triangles[12];

GLuint program; // Shader
mat4 projectionMatrix;

static GLfloat pos[4] = { 5.0, 5.0, 10.0, 0.0 };
static GLfloat red[4] = { 0.8, 0.1, 0.0, 1.0 };
static GLfloat green[4] = { 0.0, 0.8, 0.2, 1.0 };
static GLfloat blue[4] = { 0.2, 0.2, 1.0, 1.0 };

static GLfloat colors[12][4] = {
									{ 0.8, 0.1, 0.0, 1.0 }, // Red
									{ 0.0, 0.8, 0.2, 1.0 }, // Green
									{ 0.2, 0.2, 1.0, 1.0 }, // Blue
									{ 0.7, 0.7, 0.7, 1.0 }, // Gray
									{ 0.7, 0.2, 0.7, 1.0 }, // Purple/magenta
									{ 1.0, 1.0, 0.0, 1.0 }, // Bright yellow
									{ 1.0, 0.6, 0.0, 1.0 }, // Orange?
									{ 0.0, 1.0, 1.0, 1.0 }, // Bright cyan
									{ 1.0, 1.0, 1.0, 1.0 }, // White
									{ 1.0, 0.0, 0.0, 1.0 }, // Bright red
									{ 0.0, 1.0, 0.0, 1.0 }, // Bright green
									{ 0.9, 0.5, 0.6, 1.0 } // Pale red
									};


static void
draw(void)
{
	printError("pre display");

	angle += 2.0;
	mat4 rx, ry, rz, globalR, t, teapotRot, m;
	int i;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
// GLOBAL ROTATION
	t = T(0, 0, -40.0);
	rx = Rx(view_rotx*M_PI/180);
	ry = Ry(-view_roty*M_PI/180);
	rz = Rz(view_rotz*M_PI/180);
	globalR = Mult(rx, ry);
	globalR = Mult(t, globalR);
	globalR = Mult(globalR, rz);
	
	for (i=0; i < 12; i++)
	{
		t = T(-5.0 + 4.0*(i/3), -6.0 + 5.0*(i%3), 0.0);
		rx = Rx(-M_PI/2);
		teapotRot = Rz(angle*M_PI/180*(i+1)/3);
		m = Mult(globalR, t);
		m = Mult(m, rx);
		m = Mult(m, teapotRot);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"), 1, GL_TRUE, m.m);
		glUniform4fv(glGetUniformLocation(program, "color"), 1, colors[i]);
		glBindVertexArray(vao[i]);	// Select VAO
		glDrawElements(GL_TRIANGLES, triangles[i], GL_UNSIGNED_INT, NULL);
	}

	glutSwapBuffers();
	glutPostRedisplay();
	
	printError("display");
}


/* new window size or exposure */
static void
reshape(int width, int height)
{
	GLfloat h = (GLfloat) height / (GLfloat) width;

	glViewport(0, 0, (GLint) width, (GLint) height);
	projectionMatrix = frustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

static void
init(void)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(1,1,1,1);
	
	program = loadShaders("tea.vert", "tea.frag");
	glUniform4fv(glGetUniformLocation(program, "pos"), 1, pos);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	printf("SHADERS LOADED\n");
	sleep(1);
	
/*	
	vec3 p260;
	VectorAdd(&teacupVertices[194], &teacupVertices[206], &p260);
	ScalarMult(&p260, 0.5, &p260);
	printf("p260 = %f %f %f\n", p260.x, p260.y, p260.z);
	
	vec3 p261;
	VectorAdd(&teacupVertices[195], &teacupVertices[223], &p261);
	ScalarMult(&p261, 0.5, &p261);
	printf("p261 = %f %f %f\n", p261.x, p261.y, p261.z);
	
	vec3 p262;
	VectorAdd(&teacupVertices[205], &teacupVertices[215], &p262);
	ScalarMult(&p262, 0.5, &p262);
	printf("p262 = %f %f %f\n", p262.x, p262.y, p262.z);
	
	vec3 p263;
	VectorAdd(&teacupVertices[214], &teacupVertices[224], &p263);
	ScalarMult(&p263, 0.5, &p263);
	printf("p263 = %f %f %f\n", p263.x, p263.y, p263.z);
*/
	
	/* make the teapots */
	vao[0] = gluggBuildBezierPatchModel(teapotVertices, teapotIndices, 0, 32, &triangles[0], program, kStep);
	vao[1] = gluggBuildBezierPatchModel(teapotVertices, teapotIndices, 0, 32, &triangles[1], program, kStep*2);
	vao[2] = gluggBuildBezierPatchModel(teapotVertices, teapotIndices, 0, 32, &triangles[2], program, kStep*5);
	/* make the teacups */
	vao[3] = gluggBuildBezierPatchModel(teacupVertices, teacupIndices, 0, 16, &triangles[3], program, kStep);
	vao[4] = gluggBuildBezierPatchModel(teacupVertices, teacupIndices, 0, 16, &triangles[4], program, kStep*2);
	vao[5] = gluggBuildBezierPatchModel(teacupVertices, teacupIndices, 0, 16, &triangles[5], program, kStep*5);
	/* make the plates */
	vao[6] = gluggBuildBezierPatchModel(teacupVertices, teacupIndices, 16, 30, &triangles[6], program, kStep);
	vao[7] = gluggBuildBezierPatchModel(teacupVertices, teacupIndices, 16, 30, &triangles[7], program, kStep*2);
	vao[8] = gluggBuildBezierPatchModel(teacupVertices, teacupIndices, 16, 30, &triangles[8], program, kStep*5);
	/* make the tea spoons */
	vao[9] = gluggBuildBezierPatchModel(teaspoonVertices, teaspoonIndices, 0, 16, &triangles[9], program, kStep);
	vao[10] = gluggBuildBezierPatchModel(teaspoonVertices, teaspoonIndices, 0, 16, &triangles[10], program, kStep*2);
	vao[11] = gluggBuildBezierPatchModel(teaspoonVertices, teaspoonIndices, 0, 16, &triangles[11], program, kStep*5);
	
	/* save the teapot */
	saveBezierPatchModel(teapotVertices, teapotIndices, 0, 32, &triangles[1], program, kStep*2, "teapot.obj");
	/* save the teacup */
	saveBezierPatchModel(teacupVertices, teacupIndices, 0, 16, &triangles[4], program, kStep*2, "teacup.obj");
	/* save the plate */
	saveBezierPatchModel(teacupVertices, teacupIndices, 16, 30, &triangles[7], program, kStep*2, "teaplate.obj");
	/* save the tea spoon */
	saveBezierPatchModel(teaspoonVertices, teaspoonIndices, 0, 16, &triangles[10], program, kStep*2, "teaspoon.obj");

	/* save the teapot body */
	saveBezierPatchModel(teapotVertices, teapotAltIndices, 0, 24, &triangles[1], program, kStep*2, "teapotbody.obj");
	/* save the teapot lid */
	saveBezierPatchModel(teapotVertices, teapotIndices, 20, 28, &triangles[1], program, kStep*2, "teapotlid.obj");
	
	projectionMatrix = frustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0);
}


void
keys(unsigned char c, int x, int y)
{
	switch (c)
	{
		case 0x1b: exit(0);
		case 'a': view_roty += 5.0; break;
		case 'd': view_roty -= 5.0; break;
		case 'w': view_rotx += 5.0; break;
		case 's': view_rotx -= 5.0; break;
	}
}


int
main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("CPU-tesselated Utah teapot, cup, plate and spoon");
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(keys);

	init();

	glutMainLoop();

	return 0;
}
