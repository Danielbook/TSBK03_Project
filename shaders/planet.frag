#version 410 core

in float noise; // Input of FS
in vec3 gsNormal;
in vec4 gsPosition;
in vec4 gsLightSourceCoord;

uniform vec3 lightPosition;
uniform float amplitude;
uniform float frequency;
uniform vec3 surfaceColor;
uniform vec3 topColor;
uniform vec3 floorColor;
uniform float avgTemp;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D textureUnit;
uniform float shade;

out vec4 outColor; // Output of FS

void main() {
//  /* PURPLE ICO */
//  float shade = abs(gsNormal.z);
//
//  outColor = vec4(shade, shade, shade, 1.0);
//
//  vec3 N = normalize(gsNormal);
//  vec3 L = lightPosition - N;
//  float df = abs(dot(N, L));
//  vec3 color = vec3(0.04f, 0.04f, 0.04f) + df * vec3(0, 0.75, 0.75);
//
//  float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
//  float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
//  color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;
//
//  outColor = vec4(color, 1.0);
//
  vec3 ambient, diffuse, finalColor;
  float kd = 5.8, ka = 0.0;

  vec3 N = normalize(gsNormal);
  vec3 lightPos = (viewMatrix * vec4(lightPosition, 1.0)).xyz;
  vec3 L = normalize(lightPos - gsPosition.xyz);
  float distance = length(lightPos.xyz - gsPosition.xyz);

  float df = max(0.0, dot(N, L));

//  df = df * (1.0 / (1.0 + (0.25 * distance * distance)));

  float shoreLineTop = max(-10, 0.02);

  // Sandy shores
  finalColor = mix(floorColor, surfaceColor, smoothstep(0.0, shoreLineTop, noise));

  // Snow on peaks
  finalColor = mix(finalColor, topColor, smoothstep(avgTemp, avgTemp+7.0, noise));

  ambient = ka * finalColor;
  diffuse = kd * finalColor * df;

  finalColor = diffuse + ambient;

  // Perform perspective division to get the actual texture position
  vec4 shadowCoordinateWdivide = gsLightSourceCoord / gsLightSourceCoord.w;

  // Used to lower moire' pattern and self-shadowing
  // The optimal value here will vary with different GPU's depending on their Z buffer resolution.
  shadowCoordinateWdivide.z -= 0.02; // 0.0005;

  float shadow = 1.0;
  float dx, dy;
  for (dy = -3.5 ; dy <= 3.5 ; dy+=1.0)
    for (dx = -3.5 ; dx <= 3.5 ; dx+=1.0)
    {
      float distanceFromLight = texture(textureUnit, shadowCoordinateWdivide.st + vec2(dx, dy)/250.0).x;
      distanceFromLight = (distanceFromLight-0.5) * 2.0;

      if (gsLightSourceCoord.w > 0.0)
        if (distanceFromLight < shadowCoordinateWdivide.z) // shadow
          shadow -= 0.5/64; // = 0.5 shadow if total shadow (for 64 samples)
    }
    outColor = vec4(shadow * shade) * vec4(finalColor, 1.0);
//    outColor = vec4(finalColor, 1.0);
}

