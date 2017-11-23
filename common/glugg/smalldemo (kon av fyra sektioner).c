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

#ifndef M_PI
#define M_PI 3.14159265
#endif

int patchIndices[][16] = 
{
	{0,9,10,1, 11,19,17,21, 12,18,20,22, 2,23,24,7},
	{0,11,12,2, 13,27,25,29, 14,26,28,30, 3,31,32,5},
	{0,13,14,3, 15,35,33,37, 16,34,36,38, 4,39,40,6},
	{0,15,16,4, 9,43,41,47, 10,42,44,48, 1,45,46,8},
};

vec3 patchVertices1[] =
{
	{0.0, 1.0, 0.0},  // Top 0
	{0.0, 0.0, 1.0},  // Back 1
	{1.0, 0.0, 0.0},  // Right 2
	{0.0, 0.0,-1.0},  // Front 3
	{-1.0,0.0, 0.0},  // Left 4
	
	{2.0,-1.0,-2.0},  // Front-right 5
	{-2.0,-1.0,-2.0},  // Front-left 6
	{2.0,-1.0, 2.0},  // Back-right 7
	{-2.0,-1.0, 2.0},  // Back-left 8
	
	// Mellan 0 och 1
	{0.0, 0.7, 0.3}, // 9
	{0.0, 0.3, 0.7}, // 10
	// Mellan 0 och 2
	{0.3, 0.7, 0.0}, // 11
	{0.7, 0.3, 0.0}, // 12
	// Mellan 0 och 3
	{0.0, 0.7,-0.3}, // 13
	{0.0, 0.3,-0.7}, // 14
	// Mellan 0 och 4
	{-0.3, 0.7, 0.0}, // 15
	{-0.7, 0.3, 0.0}, // 16

// Två diagonaler i 0172!
	// Mellan 1 och 2
	{0.3, 0.0, 0.7}, // 17
	{0.7, 0.0, 0.3}, // 18
	// Mellan 0 och 7
	{0.7, 0.3, 0.7},  // 19
	{1.3,-0.3, 1.3},  // 20

// Bottenkanter på 0172
	// Mellan 1 och 7
	{0.7,-0.3, 1.3},  // 21
	{1.3,-0.7, 1.7},  // 22
	// Mellan 2 och 7
	{1.3,-0.3, 0.7},  // 23
	{1.7,-0.7, 1.3},  // 24

// Två diagonaler i 0253!
	// Mellan 2 och 3
	{0.7, 0.0,-0.3},  // 25
	{0.3, 0.0,-0.7},  // 26
	// Mellan 0 och 5
	{0.7, 0.3, 0.7},  // 27
	{1.3,-0.3,-0.7},  // 28

// Bottenkanter på 0253
	// Mellan 2 och 5
	{1.3,-0.3,-0.7},  // 29
	{1.7,-0.7,-1.3},  // 30
	// Mellan 3 och 5
	{0.7,-0.3,-1.3},  // 31
	{1.3,-0.7,-1.7},  // 32

// Två diagonaler i 0364!
	// Mellan 3 och 4
	{-0.3, 0.0,-0.7},  // 33
	{-0.7,0.0,-0.3},  // 34
	// Mellan 0 och 6
	{-0.7, 0.3,-0.7},  // 35
	{-1.3,-0.3,-1.3},  // 36

// Bottenkanter på 0364
	// Mellan 3 och 6
	{-0.7,-0.3,-1.3},  // 37
	{-1.3,-0.7,-1.7},  // 38
	// Mellan 4 och 6
	{-1.3,-0.3,-0.7},  // 39
	{-1.7,-0.7,-1.3},  // 40

// Två diagonaler i 0481!
	// Mellan 1 och 4
	{-0.3, 0.0, 0.7},  // 41
	{-0.7,0.0, 0.3},  // 42
	// Mellan 0 och 8
	{-0.7, 0.3, 0.7},  // 43
	{-1.3,-0.3, 1.3},  // 44

// Bottenkanter på 0481
	// Mellan 1 och 8
	{-0.7,-0.3, 1.3},  // 45
	{-1.3,-0.7, 1.7},  // 46
	// Mellan 4 och 8
	{-1.3,-0.3, 0.7},  // 47
	{-1.7,-0.7, 1.3},  // 48
};

vec3 patchVertices2[] =
{
	{0.0, 1.0, 0.0},  // Top 0
	{0.0, 0.0, 1.0},  // Back 1
	{1.0, 0.0, 0.0},  // Right 2
	{0.0, 0.0,-1.0},  // Front 3
	{-1.0,0.0, 0.0},  // Left 4
	
	{2.0,-1.0,-2.0},  // Front-right 5
	{-2.0,-1.0,-2.0},  // Front-left 6
	{2.0,-1.0, 2.0},  // Back-right 7
	{-2.0,-1.0, 2.0},  // Back-left 8
	
	// Mellan 0 och 1
	{0.0, 1.0, 0.3}, // 9
	{0.0, 0.3, 0.7}, // 10
	// Mellan 0 och 2
	{0.3, 1.0, 0.0}, // 11
	{0.7, 0.3, 0.0}, // 12
	// Mellan 0 och 3
	{0.0, 1.0,-0.3}, // 13
	{0.0, 0.3,-0.7}, // 14
	// Mellan 0 och 4
	{-0.3, 1.0, 0.0}, // 15
	{-0.7, 0.3, 0.0}, // 16

// Två diagonaler i 0172!
	// Mellan 1 och 2
	{0.3, 0.0, 0.7}, // 17
	{0.7, 0.0, 0.3}, // 18
	// Mellan 0 och 7
	{0.7, 1.0, 0.7},  // 19
	{1.3,-0.3, 1.3},  // 20

// Bottenkanter på 0172
	// Mellan 1 och 7
	{0.7,-0.3, 1.3},  // 21
	{1.3,-0.7, 1.7},  // 22
	// Mellan 2 och 7
	{1.3,-0.3, 0.7},  // 23
	{1.7,-0.7, 1.3},  // 24

// Två diagonaler i 0253!
	// Mellan 2 och 3
	{0.7, 0.0,-0.3},  // 25
	{0.3, 0.0,-0.7},  // 26
	// Mellan 0 och 5
	{0.7, 1.0, 0.7},  // 27
	{1.3,-0.3,-0.7},  // 28

// Bottenkanter på 0253
	// Mellan 2 och 5
	{1.3,-0.3,-0.7},  // 29
	{1.7,-0.7,-1.3},  // 30
	// Mellan 3 och 5
	{0.7,-0.3,-1.3},  // 31
	{1.3,-0.7,-1.7},  // 32

// Två diagonaler i 0364!
	// Mellan 3 och 4
	{-0.3, 0.0,-0.7},  // 33
	{-0.7,0.0,-0.3},  // 34
	// Mellan 0 och 6
	{-0.7, 1.0,-0.7},  // 35
	{-1.3,-0.3,-1.3},  // 36

// Bottenkanter på 0364
	// Mellan 3 och 6
	{-0.7,-0.3,-1.3},  // 37
	{-1.3,-0.7,-1.7},  // 38
	// Mellan 4 och 6
	{-1.3,-0.3,-0.7},  // 39
	{-1.7,-0.7,-1.3},  // 40

// Två diagonaler i 0481!
	// Mellan 1 och 4
	{-0.3, 0.0, 0.7},  // 41
	{-0.7,0.0, 0.3},  // 42
	// Mellan 0 och 8
	{-0.7, 1.0, 0.7},  // 43
	{-1.3,-0.3, 1.3},  // 44

// Bottenkanter på 0481
	// Mellan 1 och 8
	{-0.7,-0.3, 1.3},  // 45
	{-1.3,-0.7, 1.7},  // 46
	// Mellan 4 och 8
	{-1.3,-0.3, 0.7},  // 47
	{-1.7,-0.7, 1.3},  // 48
};

/*
void CalcInbetweenies()
{
	int x, y, i;
	// If we know that the four corners exist, interpolate the ones in between
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++)
		{
			i = x*4 + y;
			x * (y - 4) * v[3]
			x * y * v[15]
			(x - 4) * (y - 4) * v[12]
		}
}
*/

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLfloat angle = 0.0;

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

	angle += 0.05;
	mat4 m;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		m = Mult(lookAt(10,0,-30, 0,0,0, 0,1,0), Mult(Rx(-0.8), Mult(Ry(angle), Rx(0.2))));
		
		glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"), 1, GL_TRUE, m.m);
		glUniform4fv(glGetUniformLocation(program, "color"), 1, color);
		glBindVertexArray(theVao);	// Select VAO
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

static void
init(void)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(1,1,1,1);
	
	program = loadShaders("tea.vert", "tea.frag");
	glUniform4fv(glGetUniformLocation(program, "pos"), 1, pos);
	projectionMatrix = frustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	printf("SHADERS LOADED\n");
	
	/* make the patch */
	theVao = gluggBuildBezierPatchModel(patchVertices2, patchIndices, 0, 4, &triangles, program, 0.1);
	
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
	glutCreateWindow("Small CPU-tesselation, 4 patches");
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(keys);
	glutRepeatingTimer(20);

	init();

	glutMainLoop();

	return 0;
}
