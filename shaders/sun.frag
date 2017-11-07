#version 410

uniform float time;

in vec3 vNormal;
in vec3 pos;
in float noise;

uniform float sunAmp;

out vec4 outColor;

void main() {
  vec3 fireColor = vec3(0.98, 0.75, 0.4);
  vec3 surfaceColor = vec3(0.72, 0.05, 0);

  float kd = 0.6;
  float ka = 0.4;
  vNormal;
  pos;

  vec3 finalColor = mix(surfaceColor, fireColor, smoothstep(0.0, 0.04, noise));

  // Low freq noise
  finalColor = finalColor-0.04*noise;

  vec3 ambient = ka * finalColor;
  vec3 diffuse = kd * finalColor;

  finalColor = ambient+diffuse;

  outColor = vec4(finalColor, 1.0);
}

