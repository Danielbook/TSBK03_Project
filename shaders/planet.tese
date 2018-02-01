#version 410 core

//layout(triangles, equal_spacing, cw) in;
layout(triangles, equal_spacing, cw) in;
in vec4 tcPosition[]; // Original patch vertices
in vec3 tcNormal[]; // Output of TC
//in vec3 tcNormal[];
//in float tcNoise[];
//in vec4 tcLightSourceCoord[];

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

out vec3 tePatchDistance;
//out vec3 tePosition;
out vec3 teNormal;

//out float teNoise;
//out float teLightSourceCoord;
//out vec3 teNormal;


void main()
{
  vec4 p0 = gl_TessCoord.x * tcPosition[0]; // Barycentric!
  vec4 p1 = gl_TessCoord.y * tcPosition[1];
  vec4 p2 = gl_TessCoord.z * tcPosition[2];

  vec3 n0 = gl_TessCoord.x * tcNormal[0]; // Barycentric!
  vec3 n1 = gl_TessCoord.y * tcNormal[1];
  vec3 n2 = gl_TessCoord.z * tcNormal[2];

  gl_Position = p0 + p1 + p2; // Sum with weights from the barycentric coords any way we like
  teNormal = n0 + n1 + n2; // Sum with weights from the barycentric coords any way we like

//  vec3 p0 = gl_TessCoord.x * tcPosition[0];
//  vec3 p1 = gl_TessCoord.y * tcPosition[1];
//  vec3 p2 = gl_TessCoord.z * tcPosition[2];
//  vec3 n0 = gl_TessCoord.x * tcNormal[0]; // Barycentric!
//  vec3 n1 = gl_TessCoord.y * tcNormal[1];
//  vec3 n2 = gl_TessCoord.z * tcNormal[2];
//  tePatchDistance = gl_TessCoord;
////  tePosition = normalize(p0 + p1 + p2);
//  teNormal = n0 + n1 + n2; // Sum with weights from the barycentric coords any way we like
//  gl_Position = projectionMatrix * modelViewMatrix * vec4(normalize(p0 + p1 + p2), 1);

//  vec3 p0 = gl_TessCoord.x * tcPosition[0]; // Barycentric!
//  vec3 p1 = gl_TessCoord.y * tcPosition[1];
//  vec3 p2 = gl_TessCoord.z * tcPosition[2];
//  gl_Position = projectionMatrix * modelViewMatrix * vec4(p0 + p1 + p2, 1); // Sum with weights from the barycentric coords any way we like
//  tePatchDistance = gl_TessCoord;

//  teNoise = tcNoise[0];
//  teLightSourceCoord = tcLightSourceCoord[0];

// Apply vertex transformation here if we want
}