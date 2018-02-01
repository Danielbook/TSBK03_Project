#version 410 core

//layout(triangles, equal_spacing, cw) in;
layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[]; // Original patch vertices
//in vec3 tcNormal[];
//in float tcNoise[];
//in vec4 tcLightSourceCoord[];

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

out vec3 tePatchDistance;
out vec3 tePosition;

//out float teNoise;
//out float teLightSourceCoord;
//out vec3 teNormal;


void main()
{
  vec3 p0 = gl_TessCoord.x * tcPosition[0];
  vec3 p1 = gl_TessCoord.y * tcPosition[1];
  vec3 p2 = gl_TessCoord.z * tcPosition[2];
  tePatchDistance = gl_TessCoord;
  tePosition = normalize(p0 + p1 + p2);
  gl_Position = projectionMatrix * modelViewMatrix * vec4(tePosition, 1);

//  vec3 p0 = gl_TessCoord.x * tcPosition[0]; // Barycentric!
//  vec3 p1 = gl_TessCoord.y * tcPosition[1];
//  vec3 p2 = gl_TessCoord.z * tcPosition[2];
//  gl_Position = projectionMatrix * modelViewMatrix * vec4(p0 + p1 + p2, 1); // Sum with weights from the barycentric coords any way we like
//  tePatchDistance = gl_TessCoord;

//  teNoise = tcNoise[0];
//  teLightSourceCoord = tcLightSourceCoord[0];

// Apply vertex transformation here if we want
}