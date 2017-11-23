#ifndef _GLUGG_
#define _GLUGG_

#ifdef __cplusplus
extern "C" {
#endif

// GLUGG interface
// GLUGG is the OpenGL Utilties for Geometry Generation
// By Ingemar Ragnemalm 2013

#include "VectorUtils3.h"


void gluggBegin(int count);
void gluggVertex(float x, float y, float z);
void gluggVertexv(Point3D p);
void gluggNormal(float x, float y, float z);
void gluggNormalv(Point3D n);
void gluggTexCoord(float s, float t);
GLuint gluggEnd(int *count, GLuint program, char optimize);
void gluggDispose();

void gluggBuildBezier(Point3D *vertices, int *indices, float step);
GLint gluggBuildBezierPatchModel(Point3D *vertices, int *indices, int startPatch, int endPatch, int *count, GLuint program, float step);

// Inspect internal variables (use before gluggEnd, which clears them!)
GLfloat *gluggGetVertexArray();
GLfloat *gluggGetNormalsArray();
GLfloat *gluggGetTexCoordArray();
GLfloat *gluggGetIndexArray();
int gluggGetNumVertices();
int gluggGetNumIndices();
void arraysToElements(); // Needed before gluggGetNumIndices

#ifdef __cplusplus
}
#endif

#endif
