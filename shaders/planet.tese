#version 410 core

layout(triangles, equal_spacing, cw) in;

in vec3 tcPosition[]; // Input of TE
in vec3 tcNormal[];
in vec4 tcLightSourceCoord[];

out vec3 tePosition; // Output of TE
out vec3 teNormal;
out vec4 teLightSourceCoord;
//out vec3 tePatchDistance;

void main()
{
  vec3 p0 = gl_TessCoord.x * tcPosition[0]; // Barycentric!
  vec3 p1 = gl_TessCoord.y * tcPosition[1];
  vec3 p2 = gl_TessCoord.z * tcPosition[2];

  vec3 n0 = gl_TessCoord.x * tcNormal[0]; // Barycentric!
  vec3 n1 = gl_TessCoord.y * tcNormal[1];
  vec3 n2 = gl_TessCoord.z * tcNormal[2];

  vec4 l0 = gl_TessCoord.x * tcLightSourceCoord[0];
  vec4 l1 = gl_TessCoord.y * tcLightSourceCoord[1];
  vec4 l2 = gl_TessCoord.z * tcLightSourceCoord[2];

  teLightSourceCoord = (l0 + l1 + l2);
  tePosition = normalize(p0 + p1 + p2);
  teNormal = n0 + n1 + n2; // Sum with weights from the barycentric coords any way we like
  gl_Position = vec4(tePosition, 1.0);
}