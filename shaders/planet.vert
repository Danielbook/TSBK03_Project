#version 410 core

in vec3 inPosition;
in vec3 inNormal;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat3 normalMatrix;

out vec3 vNormal; // Output of VS
out vec3 vPosition;
out vec4 vLightSourceCoord;

void main() {
  vPosition = inPosition;
  vNormal = inNormal;
  vLightSourceCoord = textureMatrix * vec4(inPosition, 1.0); // Transform vertex to light source coords
}