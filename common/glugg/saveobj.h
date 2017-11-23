#ifndef SAVE_OBJ
#define SAVE_OBJ

#include "loadobj.h"
#include "VectorUtils3.h"

void SaveOBJ(
	GLfloat* vertexArray,
	GLfloat* normalArray,
	GLfloat* texCoordArray,
	GLuint* indexArray,
	int numVertices,
	int numIndices,
	char *filename);
	
void SaveModel(Model *m, char *filename);
void saveBezierPatchModel(vec3 *vertices, int *indices,
						int startPatch, int endPatch,
						int *count, GLuint program, float step, char *fileName);

#endif
