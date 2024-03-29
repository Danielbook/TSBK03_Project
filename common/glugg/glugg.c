// GLUGG, OpenGL Utility for Geometry Generation
// By Ingemar Ragnemalm 2013

// This file implements a package to generate geometry and pass it
// to the GPU, in a way similar to the classic Immediate Mode but
// without the drawback of massive numbers of function calls every frame.
// In a way, it is like using immediate mode to generate display lists.

// This package is particularly well suited for porting old OpenGL code.
// I developed early versions of the code when porting the "gears" demo
// (the one that later evolved to glxgears) and the Utah Teapot to modern
// OpenGL code. Then I repackaged it as reusable code.

// 130226: First version, based on my earlier gears and teapot demos.
// Can only output triangles for glDrawArrays. (No glDrawElements yet.)
// 130310: Support for glDrawElements (with optional hash table to speed the
// re-organization up that seems to matter little).

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
//	#include <GLUT/glut.h>
// uses framework Cocoa
#else
	#include <GL/glut.h>
#endif
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "glugg.h"

// ----------------------- Polyhedra builder ------------------------
// Somewhat similar to immediate mode

static GLfloat *gVertices;
static GLfloat *gNormals;
static GLfloat *gTexCoord;
static GLuint *gIndices;
static int numVertices, allocatedVertices;
static int numIndices;
static float currentNormalsX, currentNormalsY, currentNormalsZ;
static float currentTexCoordS, currentTexCoordT;

// gluggBegin sort of corresponds to glBegin
// Does not yet support different kinds of primitives
void gluggBegin(int count)
{
	if ((count <= 0) || (count > 100000000)) count = 1000;
	gVertices = (GLfloat *) malloc(count*3*sizeof(GLfloat));
	gNormals = (GLfloat *) malloc(count*3*sizeof(GLfloat));
	gTexCoord = (GLfloat *) malloc(count*2*sizeof(GLfloat));
	gIndices = NULL;
	allocatedVertices = count;
	numVertices = 0;
}

// gluggVertex can directly replace glVertex
void gluggVertex(float x, float y, float z)
{
	if (numVertices+1 > allocatedVertices)
	{
		allocatedVertices *= 2;
		gVertices = realloc(gVertices, 3*allocatedVertices*sizeof(GLfloat));
		gNormals = realloc(gNormals, 3*allocatedVertices*sizeof(GLfloat));
		gTexCoord = realloc(gTexCoord, 2*allocatedVertices*sizeof(GLfloat));
		if ( gVertices == NULL || gNormals==NULL || gTexCoord==NULL) //realloc failed!
		{
			printf("fatal error, could not realloc!\n");
			exit(1);
		}
		else
			printf("realloc to %d vertices\n", allocatedVertices);
	}

	gVertices[numVertices*3] = x;
	gVertices[numVertices*3+1] = y;
	gVertices[numVertices*3+2] = z;
	gNormals[numVertices*3] = currentNormalsX;
	gNormals[numVertices*3+1] = currentNormalsY;
	gNormals[numVertices*3+2] = currentNormalsZ;
	gTexCoord[numVertices*2] = currentTexCoordS;
	gTexCoord[numVertices*2+1] = currentTexCoordT;
	numVertices += 1;
}

// gluggVertexv (glVertex#v)
void gluggVertexv(vec3 p)
{
	gluggVertex(p.x, p.y, p.z);
}

// gluggNormal (glNormal)
void gluggNormal(float x, float y, float z)
{
	currentNormalsX = x;
	currentNormalsY = y;
	currentNormalsZ = z;
}

// gluggNormalv (glNormalv)
void gluggNormalv(vec3 n)
{
	currentNormalsX = n.x;
	currentNormalsY = n.y;
	currentNormalsZ = n.z;
}

// gluggTexCoord (glTexCoord)
void gluggTexCoord(float s, float t)
{
	currentTexCoordS = s;
	currentTexCoordT = t;
}

void arraysToElements()
{
	// For every input vertex, search output (hash?) for any other vertex with the same data.
	// If none exists, add a new one.
	// Add index to index array.
	// For a first version, don't hash, just linear search. (Way too slow!)
	
	if (gIndices != NULL) { free(gIndices); gIndices = NULL;}
	
	GLfloat *newVertices = (GLfloat *) malloc(3*allocatedVertices*sizeof(GLfloat));
	GLfloat *newNormals = (GLfloat *) malloc(3*allocatedVertices*sizeof(GLfloat));
	GLfloat *newTexCoord = (GLfloat *) malloc(2*allocatedVertices*sizeof(GLfloat));
	gIndices = (GLuint *) malloc(allocatedVertices*sizeof(GLuint));
	GLuint *indexHash = (GLuint *) malloc(allocatedVertices*sizeof(GLuint));
	long numNewVertices = 0;
	numIndices = 0;
	int i,j,s;

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
#define HASH

	// Find range of x+y (less likely to be aligned than x or y)
	float maxxy = newVertices[0]+newVertices[1];
	float minxy = newVertices[0]+newVertices[1];
	for (j = 0; j < numNewVertices; j++)
	{
		maxxy = max(maxxy, newVertices[j*3]+newVertices[j*3+1]);
		minxy = min(maxxy, newVertices[j*3]+newVertices[j*3+1]);
	}
	
	for (j = 0; j < allocatedVertices; j++)
	{
		indexHash[j] = -1;
	}

	for (i = 0; i < numVertices; i++)
	{
#ifdef HASH
	
	// Search with hash
		int h = (int)(newVertices[j*3]+newVertices[j*3+1] - minxy) / (maxxy-minxy) * allocatedVertices;
		h = min(h, allocatedVertices-1);
		h = max(h, 0);

		s = -1;
		for (; h < numNewVertices && s < 0; h++)
		{
			if (indexHash[h] != -1)
			{
				if (newVertices[indexHash[h]*3] == gVertices[i*3] &&
					newVertices[indexHash[h]*3+1] == gVertices[i*3+1] &&
					newVertices[indexHash[h]*3+2] == gVertices[i*3+2] &&
					newNormals[indexHash[h]*3] == gNormals[i*3] &&
					newNormals[indexHash[h]*3+1] == gNormals[i*3+1] &&
					newNormals[indexHash[h]*3+2] == gNormals[i*3+2] &&
					newTexCoord[indexHash[h]*2] == gTexCoord[i*2] &&
					newTexCoord[indexHash[h]*2+1] == gTexCoord[i*2+1])
				{
					s = indexHash[h]; // Found one!
					break;
				}
				else
				{
				}
			}
			else
			{ // Found empty slot - grab it!
				indexHash[h] = numNewVertices;
				break;
			}
		}
		
#else

	// Search without hash
		s = -1;
		for (j = 0; j < numNewVertices && s < 0; j++)
		{
			if (newVertices[j*3] == gVertices[i*3] &&
				newVertices[j*3+1] == gVertices[i*3+1] &&
				newVertices[j*3+2] == gVertices[i*3+2] &&
				newNormals[j*3] == gNormals[i*3] &&
				newNormals[j*3+1] == gNormals[i*3+1] &&
				newNormals[j*3+2] == gNormals[i*3+2] &&
				newTexCoord[j*2] == gTexCoord[i*2] &&
				newTexCoord[j*2+1] == gTexCoord[i*2+1])
			{
				s = j;
			}
		}
#endif
		
		if (s < 0) // None found, insert new one
		{
			newVertices[numNewVertices*3] = gVertices[i*3];
			newVertices[numNewVertices*3+1] = gVertices[i*3+1];
			newVertices[numNewVertices*3+2] = gVertices[i*3+2];
			newNormals[numNewVertices*3] = gNormals[i*3];
			newNormals[numNewVertices*3+1] = gNormals[i*3+1];
			newNormals[numNewVertices*3+2] = gNormals[i*3+2];
			newTexCoord[numNewVertices*2] = gTexCoord[i*2];
			newTexCoord[numNewVertices*2+1] = gTexCoord[i*2+1];
			s = numNewVertices;
			numNewVertices += 1;
		}
		gIndices[numIndices] = s;
		numIndices += 1;
	}
	
	free(gVertices);
	free(gNormals);
	free(gTexCoord);
	
	gVertices = newVertices;
	gNormals = newNormals;
	gTexCoord = newTexCoord;
}


// Plan: GluggGeometryData sparar info om alla buffrar.
// D� kan man �ndra shadervariabler f�r varje buffer n�r man vill.

typedef struct
{
	GLuint vertexArrayObjID; // ID of entire record
	GLuint vertexBufferObjID;
	GLuint normalsBufferObjID;
	GLuint texCoordBufferObjID;
	GLuint indexBufferObjID;
	char useDrawElements;
} GluggGeometryData;

static int geomCount = 0, geomAllocated = 0;
#define kGeomAllocNum 16
static GluggGeometryData *geomList = NULL; // Dynamic array of GluggGeometryData
// EJ TESTAD! Skall inf�ras i gluggEnd!

static GluggGeometryData* NewGeometryData()
{
	if (geomList == NULL)
	{
		geomList = (GluggGeometryData *) malloc(kGeomAllocNum * sizeof(GluggGeometryData));
		geomAllocated = kGeomAllocNum;
		return &geomList[0];
		geomCount = 1;
	}
	else
	{
		if (geomCount+1 < geomAllocated)
		{
			return &geomList[geomCount++];
		}
		else
		{
			geomAllocated += kGeomAllocNum;
			geomList = realloc(geomList, geomAllocated*sizeof(GluggGeometryData));
			return &geomList[geomCount++];
		}
	}
}

GluggGeometryData* GetGeometryDataForVAO(GLint vao)
{
	int i;
// Search list for a vao that matches
	for (i = 0; i < geomCount; i++)
	{
		if (geomList[i].vertexArrayObjID == vao)
			return &geomList[i];
	}
	return NULL;
}

// gluggEnd is somewhat like glEnd, but returns a VAO identifier and a length.
GLuint gluggEnd(int *count, GLuint program, char optimize)
{
	// Build VAO and VBOs
	GLuint vertexArrayObjID;
	GLuint vertexBufferObjID;
	GLuint normalsBufferObjID;
	GLuint texCoordBufferObjID;
	GLuint indexBufferObjID;

	if (optimize)
		arraysToElements();
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &normalsBufferObjID);
	glGenBuffers(1, &texCoordBufferObjID);
	if (gIndices != NULL) glGenBuffers(1, &indexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, numVertices*3*sizeof(GLfloat), gVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
	
	// VBO for normals data
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, numVertices*3*sizeof(GLfloat), gNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));
	
	// VBO for texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, numVertices*2*sizeof(GLfloat), gTexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_TexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_TexCoord"));
	
	if (gIndices != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), gIndices, GL_STATIC_DRAW);
	}

	free(gVertices);
	free(gNormals);
	free(gTexCoord);
	if (gIndices != NULL)
	{
		free(gIndices);
		*count = numIndices;
		return vertexArrayObjID;
	}
	
	// Return VAO and length
	*count = numVertices;
	return vertexArrayObjID;
}

// Clear buffers without creating a VAO
void gluggDispose()
{
	free(gVertices);
	free(gNormals);
	free(gTexCoord);
	if (gIndices != NULL)
	{
		free(gIndices);
	}
}

// -------------------- Bezier surface tesselator --------------------------

// 2D Bezier surface lookup
void bez(vec3 *vert, int *ind, float u, float v, vec3 *p, vec3 *n)
{
	float u0, u1, u2, u3, v0, v1, v2, v3;
	float du0, du1, du2, du3, dv0, dv1, dv2, dv3;
	vec3 p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;
	vec3 uu0, uu1, uu2, uu3, uu4, uu5, uu6, uu7, uu8, uu9, uu10, uu11, uu12, uu13, uu14, uu15;
	vec3 vv0, vv1, vv2, vv3, vv4, vv5, vv6, vv7, vv8, vv9, vv10, vv11, vv12, vv13, vv14, vv15;
	vec3 uu, vv, dummyp;
	
	// Weights for positions
	u0 = (1-u)*(1-u)*(1-u);
	u1 = 3*u*(1-u)*(1-u);
	u2 = 3*u*u*(1-u);
	u3 = u*u*u;
	v0 = (1-v)*(1-v)*(1-v);
	v1 = 3*v*(1-v)*(1-v);
	v2 = 3*v*v*(1-v);
	v3 = v*v*v;
	
	// Partial derivatives, used for calculating normal vector
	du0 = -3*(1-u)*(1-u);
	du1 = 3*((1-u)*(1-u) - 2*u*(1-u));
	du2 = 3*( 2*u*(1-u) -u*u );
	du3 = 3*u*u;
	dv0 = -3*(1-v)*(1-v);
	dv1 = 3*((1-v)*(1-v) - 2*v*(1-v));
	dv2 = 3*( 2*v*(1-v) -v*v );
	dv3 = 3*v*v;

	// Get all control points
	p0 = vert[ind[0]];
	p1 = vert[ind[1]];
	p2 = vert[ind[2]];
	p3 = vert[ind[3]];
	p4 = vert[ind[4]];
	p5 = vert[ind[5]];
	p6 = vert[ind[6]];
	p7 = vert[ind[7]];
	p8 = vert[ind[8]];
	p9 = vert[ind[9]];
	p10 = vert[ind[10]];
	p11 = vert[ind[11]];
	p12 = vert[ind[12]];
	p13 = vert[ind[13]];
	p14 = vert[ind[14]];
	p15 = vert[ind[15]];

// Calculate position		
	p0 = ScalarMult(p0, u0*v0);
	p1 = ScalarMult(p1, u1*v0);
	p2 = ScalarMult(p2, u2*v0);
	p3 = ScalarMult(p3, u3*v0);
	p4 = ScalarMult(p4, u0*v1);
	p5 = ScalarMult(p5, u1*v1);
	p6 = ScalarMult(p6, u2*v1);
	p7 = ScalarMult(p7, u3*v1);
	p8 = ScalarMult(p8, u0*v2);
	p9 = ScalarMult(p9, u1*v2);
	p10 = ScalarMult(p10, u2*v2);
	p11 = ScalarMult(p11, u3*v2);
	p12 = ScalarMult(p12, u0*v3);
	p13 = ScalarMult(p13, u1*v3);
	p14 = ScalarMult(p14, u2*v3);
	p15 = ScalarMult(p15, u3*v3);
	*p = VectorAdd(p0, p1);
	*p = VectorAdd(p2, *p);
	*p = VectorAdd(p3, *p);
	*p = VectorAdd(p4, *p);
	*p = VectorAdd(p5, *p);
	*p = VectorAdd(p6, *p);
	*p = VectorAdd(p7, *p);
	*p = VectorAdd(p8, *p);
	*p = VectorAdd(p9, *p);
	*p = VectorAdd(p10, *p);
	*p = VectorAdd(p11, *p);
	*p = VectorAdd(p12, *p);
	*p = VectorAdd(p13, *p);
	*p = VectorAdd(p14, *p);
	*p = VectorAdd(p15, *p);

// Calculate normal
	p0 = vert[ind[0]];
	p1 = vert[ind[1]];
	p2 = vert[ind[2]];
	p3 = vert[ind[3]];
	p4 = vert[ind[4]];
	p5 = vert[ind[5]];
	p6 = vert[ind[6]];
	p7 = vert[ind[7]];
	p8 = vert[ind[8]];
	p9 = vert[ind[9]];
	p10 = vert[ind[10]];
	p11 = vert[ind[11]];
	p12 = vert[ind[12]];
	p13 = vert[ind[13]];
	p14 = vert[ind[14]];
	p15 = vert[ind[15]];

// Contributions to d/du
	uu0 = ScalarMult(p0, du0*v0);
	uu1 = ScalarMult(p1, du1*v0);
	uu2 = ScalarMult(p2, du2*v0);
	uu3 = ScalarMult(p3, du3*v0);
	uu4 = ScalarMult(p4, du0*v1);
	uu5 = ScalarMult(p5, du1*v1);
	uu6 = ScalarMult(p6, du2*v1);
	uu7 = ScalarMult(p7, du3*v1);
	uu8 = ScalarMult(p8, du0*v2);
	uu9 = ScalarMult(p9, du1*v2);
	uu10 = ScalarMult(p10, du2*v2);
	uu11 = ScalarMult(p11, du3*v2);
	uu12 = ScalarMult(p12, du0*v3);
	uu13 = ScalarMult(p13, du1*v3);
	uu14 = ScalarMult(p14, du2*v3);
	uu15 = ScalarMult(p15, du3*v3);
	
// Contributions to d/dv
	vv0 = ScalarMult(p0, u0*dv0);
	vv1 = ScalarMult(p1, u1*dv0);
	vv2 = ScalarMult(p2, u2*dv0);
	vv3 = ScalarMult(p3, u3*dv0);
	vv4 = ScalarMult(p4, u0*dv1);
	vv5 = ScalarMult(p5, u1*dv1);
	vv6 = ScalarMult(p6, u2*dv1);
	vv7 = ScalarMult(p7, u3*dv1);
	vv8 = ScalarMult(p8, u0*dv2);
	vv9 = ScalarMult(p9, u1*dv2);
	vv10 = ScalarMult(p10, u2*dv2);
	vv11 = ScalarMult(p11, u3*dv2);
	vv12 = ScalarMult(p12, u0*dv3);
	vv13 = ScalarMult(p13, u1*dv3);
	vv14 = ScalarMult(p14, u2*dv3);
	vv15 = ScalarMult(p15, u3*dv3);
	
	uu = VectorAdd(uu0, uu1);
	uu = VectorAdd(uu, uu2);
	uu = VectorAdd(uu, uu3);
	uu = VectorAdd(uu, uu4);
	uu = VectorAdd(uu, uu5);
	uu = VectorAdd(uu, uu6);
	uu = VectorAdd(uu, uu7);
	uu = VectorAdd(uu, uu8);
	uu = VectorAdd(uu, uu9);
	uu = VectorAdd(uu, uu10);
	uu = VectorAdd(uu, uu11);
	uu = VectorAdd(uu, uu12);
	uu = VectorAdd(uu, uu13);
	uu = VectorAdd(uu, uu14);
	uu = VectorAdd(uu, uu15);

	vv = VectorAdd(vv0, vv1);
	vv = VectorAdd(vv, vv2);
	vv = VectorAdd(vv, vv3);
	vv = VectorAdd(vv, vv4);
	vv = VectorAdd(vv, vv5);
	vv = VectorAdd(vv, vv6);
	vv = VectorAdd(vv, vv7);
	vv = VectorAdd(vv, vv8);
	vv = VectorAdd(vv, vv9);
	vv = VectorAdd(vv, vv10);
	vv = VectorAdd(vv, vv11);
	vv = VectorAdd(vv, vv12);
	vv = VectorAdd(vv, vv13);
	vv = VectorAdd(vv, vv14);
	vv = VectorAdd(vv, vv15);
	
	*n = CrossProduct(uu, vv);
	if (Norm(*n) < 0.0001)
	{
		bez(vert, ind, (u+0.1)*0.9, (v+0.1)*0.9, &dummyp, n);
	}
	else
		*n = Normalize(*n);
}

// gluggBuildBezier is somewhat similar to glMap/glEvalCoord (in 2D).
void gluggBuildBezier(vec3 *vertices, int *indices, float step)
{
	GLfloat u, v;
	vec3 p0, p1, p2, p3, nn0, nn1, nn2, nn3;
	
	for (u = 0.0; u < 1.0; u += step)
	for (v = 0.0; v < 1.0; v += step)
	{
		bez(vertices, indices, u, v, &p0, &nn0);
		bez(vertices, indices, u+step, v, &p1, &nn1);
		bez(vertices, indices, u+step, v+step, &p2, &nn2);
		bez(vertices, indices, u, v+step, &p3, &nn3);
		
		gluggTexCoord(u, v);
		gluggNormalv(nn0);
		gluggVertexv(p0);
		gluggTexCoord(u+step, v);
		gluggNormalv(nn1);
		gluggVertexv(p1);
		gluggTexCoord(u+step, v+step);
		gluggNormalv(nn2);
		gluggVertexv(p2);
		
		gluggTexCoord(u, v);
		gluggNormalv(nn0);
		gluggVertexv(p0);
		gluggTexCoord(u+step, v+step);
		gluggNormalv(nn2);
		gluggVertexv(p2);
		gluggTexCoord(u, v+step);
		gluggNormalv(nn3);
		gluggVertexv(p3);
	}
}

// Build model from array of patches
// vertices: array of vec3
// indices: Patches given as groups of 16 indices into the vertices array.
// startPatch, endPatch: Which patches to generate (given as number of patch, not index).
// count: in: Expected number of vertices. out: Actual number.
// program: Shader to attach the VAO to. Assumes names on attributes in shader: "in_Position", "in_Normal" and "in_TexCoord"
// step: Step length in patch, must be between 0 and 1, usually something like 0.1.

GLint gluggBuildBezierPatchModel(vec3 *vertices, int *indices, int startPatch, int endPatch, int *count, GLuint program, float step)
{
	GLint i;
//	GLfloat u, v;
	GLuint vertexArrayObjID;
	
	gluggBegin(*count);
	
	// Building phase
	gluggNormal(0.0, 0.0, 1.0);	
	for (i = startPatch; i < endPatch; i++)
	{
		gluggBuildBezier(vertices, &indices[i*16], step);
	}
	
	vertexArrayObjID = gluggEnd(count, program, 1);
	printf("Laddade %d vertices.\n", *count);
	
	return vertexArrayObjID;
}

// ------------------------------------------------------


// Inspect internal variables
// Call arraysToElements before asking for indices

GLfloat *gluggGetVertexArray()
{
	return gVertices;
}

GLfloat *gluggGetNormalsArray()
{
	return gNormals;
}

GLfloat *gluggGetTexCoordArray()
{
	return gTexCoord;
}

GLfloat *gluggGetIndexArray()
{
	return gIndices;
}

int gluggGetNumVertices()
{
	return numVertices;
}

int gluggGetNumIndices()
{
	return numIndices;
}
