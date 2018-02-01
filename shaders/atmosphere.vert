#version 410
in vec3 inPosition;
in vec3 inNormal;

uniform float time;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

out vec3 vNormal;
out vec3 vPosition;

void main() {
//	vNormal = normal;
//	pos = position+normal*atmosphereHeight;

	vNormal = normalize(normalMatrix * inNormal);
  vPosition = inPosition;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vPosition, 1.0 );
}