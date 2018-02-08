#version 410 core

in vec4 inPosition;
in vec3 inNormal;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat3 normalMatrix;

out vec3 vNormal; // Output of VS
out vec4 vLightSourceCoord;

void main() {
  gl_Position = inPosition;
  vNormal = inNormal;
  vLightSourceCoord = textureMatrix * inPosition; // Transform vertex to light source coords
}