#version 410
in vec3 inPosition;
in vec3 inNormal;

uniform float amplitude;
uniform float frequency;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;

//out vec3 vNormal;
out vec3 vPosition;
//out float vNoise;
//out vec4 vLightSourceCoord;

void main() {
  inNormal;


//  vec4 viewModelPosition = modelViewMatrix * vec4(pos, 1.0);
  vec4 viewModelPosition = modelViewMatrix * vec4(inPosition, 1.0);

  vPosition = viewModelPosition.xyz;

//  vLightSourceCoord = textureMatrix * vec4(inPosition, 1.0); // Transform vertex to light source coo

//  vNormal = normalize(normalMatrix * inNormal);
}