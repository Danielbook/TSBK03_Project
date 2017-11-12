#version 410

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec2 texCoord[3];   //[3] because this makes a triangle
in vec3 vNormal[3];
in float noise[3];
in vec3 pos[3];

out vec3 vNormalG;
out vec2 texCoordG;
out float noiseG;
out vec3 posG;


void main() {
  vec4 middleOfTriangle = vec4(0.0);

  noiseG = noise[1];
  posG = pos[1];

  for(int i = 0; i < gl_in.length(); i++)
  {
    middleOfTriangle += gl_in[i].gl_Position;
  }
  middleOfTriangle /= gl_in.length();

  for(int i = 0; i < gl_in.length(); i++)
  {
    gl_Position = (gl_in[i].gl_Position + middleOfTriangle) / 2.0;
    texCoordG = texCoord[i];
    vNormalG = vNormal[i];
    EmitVertex();
  }

  // done with the vertex
  EndPrimitive();
}
