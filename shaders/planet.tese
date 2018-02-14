#version 410 core

layout(triangles, equal_spacing, cw) in;

in vec3 tcNormal[];  // Input of TE

out vec3 teNormal; // Output of TE

void main()
{
  vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz; // Barycentric!
  vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
  vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;

  vec3 n0 = gl_TessCoord.x * tcNormal[0]; // Barycentric!
  vec3 n1 = gl_TessCoord.y * tcNormal[1];
  vec3 n2 = gl_TessCoord.z * tcNormal[2];

  teNormal = n0 + n1 + n2; // Sum with weights from the barycentric coords any way we like

  // Normalize to push new vertices onto the sphere's surface
  gl_Position = vec4(normalize(p0 + p1 + p2), 1.0);
}