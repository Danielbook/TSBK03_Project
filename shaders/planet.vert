#version 410
in vec3 inPosition;
in vec3 inNormal;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 viewMatrix;

out vec3 vNormal;
out vec4 vPosition;
//out float vNoise;
//out vec4 vLightSourceCoord;

void main() {
//  float vNoise = amplitude*pnoise(frequency*inPosition, vec3(20.0));

//  vec3 pos = inPosition + vNoise * inNormal;

//  vec4 viewModelPosition = modelViewMatrix * vec4(pos, 1.0);
//  vec4 viewModelPosition = modelViewMatrix * vec4(inPosition, 1.0);

  vPosition = projectionMatrix * viewMatrix * vec4(inPosition, 1.0);
  vNormal = mat3(projectionMatrix * viewMatrix) * inNormal;

//  vLightSourceCoord = textureMatrix * vec4(inPosition, 1.0); // Transform vertex to light source coo

//  vNormal = normalize(normalMatrix * inNormal);
}