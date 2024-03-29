#version 410 core

in vec4 inPosition;
in vec3 inNormal;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat3 normalMatrix;
uniform vec3 camera;

out vec3 vNormal; // Output of VS
out vec4 vLightSourceCoord;
out float vDistance;

void main() {

  vDistance = distance(camera, inPosition.xyz);
  gl_Position = inPosition;
  vNormal = inNormal;
}