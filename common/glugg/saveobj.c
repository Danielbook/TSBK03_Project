#include "loadobj.h"
#include "saveobj.h"
#include "glugg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SaveOBJ(
	GLfloat* vertexArray,
	GLfloat* normalArray,
	GLfloat* texCoordArray,
	GLuint* indexArray,
	int numVertices,
	int numIndices,
	char *filename)
{
	FILE *file;
	int i;
	
// open file and check for errors
	file = fopen(filename, "w");
	if (file == NULL)
		return(-1);

	for (i = 0; i < numVertices; i++)
	{
		fprintf(file, "v %f %f %f\n", vertexArray[i*3], vertexArray[i*3+1], vertexArray[i*3+2]);
	}
	if (normalArray != NULL)
	for (i = 0; i < numVertices; i++)
	{
		fprintf(file, "vn %f %f %f\n", normalArray[i*3], normalArray[i*3+1], normalArray[i*3+2]);
	}
	if (texCoordArray != NULL)
	for (i = 0; i < numVertices; i++)
	{
		fprintf(file, "vt %f %f\n", texCoordArray[i*2], texCoordArray[i*2+1]);
	}
	if (indexArray != NULL)
	for (i = 0; i < numIndices/3; i++) // all triangles
	{
		if (texCoordArray != NULL)
			if (normalArray != NULL)
			{
				fprintf(file, "f %d/%d/%d", indexArray[i*3]+1, indexArray[i*3]+1, indexArray[i*3]+1); // All three
				fprintf(file, " %d/%d/%d", indexArray[i*3+1]+1, indexArray[i*3+1]+1, indexArray[i*3+1]+1); // All three
				fprintf(file, " %d/%d/%d\n", indexArray[i*3+2]+1, indexArray[i*3+2]+1, indexArray[i*3+2]+1); // All three
			}
			else // tex coords but no normals
			{
				fprintf(file, "f %d//%d", indexArray[i*3]+1, indexArray[i*3]+1);
				fprintf(file, " %d//%d", indexArray[i*3+1]+1, indexArray[i*3+1]+1);
				fprintf(file, " %d//%d\n", indexArray[i*3+2]+1, indexArray[i*3+2]+1);
			}
		else // no tex
			if (normalArray != NULL) // but we have normals
			{
				fprintf(file, "f %d/%d/", indexArray[i*3]+1, indexArray[i*3]+1);
				fprintf(file, " %d/%d/", indexArray[i*3+1]+1, indexArray[i*3+1]+1);
				fprintf(file, " %d/%d/\n", indexArray[i*3+2]+1, indexArray[i*3+2]+1);
			}
			else // no tex, no normals
			{
				fprintf(file, "f %d//", indexArray[i*3]+1);
				fprintf(file, " %d//", indexArray[i*3+1]+1);
				fprintf(file, " %d//\n", indexArray[i*3+2]+1);
			}
	}
	fclose(file);
}

void SaveModel(Model *m, char *filename)
{
	SaveOBJ(m->vertexArray,
		m->normalArray,
		m->texCoordArray,
		m->indexArray,
		m->numVertices,
		m->numIndices,
		filename);
}

// Use glugg to create bezier based models
void saveBezierPatchModel(vec3 *vertices, int *indices, int startPatch, int endPatch, int *count, GLuint program, float step, char *fileName)
{
	GLint i;
	
	gluggBegin(*count);
	
	// Building phase
	gluggNormal(0.0, 0.0, 1.0);	
	for (i = startPatch; i < endPatch; i++)
	{
		gluggBuildBezier(vertices, &indices[i*16], step);
	}
	
	arraysToElements();
	SaveOBJ(
		gluggGetVertexArray(),
		gluggGetNormalsArray(),
		gluggGetTexCoordArray(),
		gluggGetIndexArray(),
		gluggGetNumVertices(),
		gluggGetNumIndices(),
		fileName);
	
	gluggDispose(); // Instead of gluggEnd.
}
