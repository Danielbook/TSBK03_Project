#version 410 core

layout(vertices = 3) out;

in vec3 vPosition[]; // From vertex shader
in vec3 vNormal[];
in vec4 vLightSourceCoord[];

uniform int TessLevelInner; // Sent from main program
uniform int TessLevelOuter1;
uniform int TessLevelOuter2;
uniform int TessLevelOuter3;

out vec3 tcPosition[]; // Output of TC
out vec3 tcNormal[];
out vec4 tcLightSourceCoord[];

#define ID gl_InvocationID

void main()
{
  tcPosition[ID] = vPosition[ID];
  tcNormal[ID] = vNormal[ID];
  tcLightSourceCoord[ID] = vLightSourceCoord[ID];

  gl_TessLevelInner[0] = TessLevelInner; // How often the inside of a patch is divided
  gl_TessLevelOuter[0] = TessLevelOuter1; // How often an edge is divided
  gl_TessLevelOuter[1] = TessLevelOuter2;
  gl_TessLevelOuter[2] = TessLevelOuter3;


//  tcNoise[0] = vNoise[0];

//  tcLightSourceCoord[0] = vLightSourceCoord[0];
//  tcLightSourceCoord[1] = vLightSourceCoord[1];
//  tcLightSourceCoord[2] = vLightSourceCoord[2];
}


