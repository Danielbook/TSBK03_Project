#version 410
in vec3 gTriDistance;
in vec3 gPatchDistance;
in float gPrimitive;
in vec3 gsNormal;
in vec4 gsPosition;

uniform vec3 lightPosition;

//out vec4 FragColor;

//in vec3 vNormal;
//in vec3 vPosition;
//in float noise;
//in vec4 lightSourceCoord;
uniform float amplitude;
uniform float frequency;
uniform vec3 surfaceColor;
uniform vec3 snowColor;
uniform vec3 sandColor;
uniform float avgTemp;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
//uniform sampler2D textureUnit;
//uniform float shade;

out vec4 outColor;

vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise, periodic variant
float pnoise(vec3 P, vec3 rep)
{
  vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
  vec3 Pi1 = mod(Pi0 + vec3(1.0), rep); // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main() {
//  float shade = abs(gsNormal.z);
//
//  outColor = vec4(shade, shade, shade, 1.0);

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

  vec3 ambient, diffuse, finalColor;
  float kd = 0.02, ka = 0.1;

  vec3 N = normalize(gsNormal);
//  vec3 L = lightPosition - N;

  vec4 lightPos = viewMatrix * vec4(lightPosition, 1.0);
  vec3 lightVector = lightPos.xyz - gsPosition.xyz;

  float df = abs(dot(N, lightVector));

  float shoreLineTop = 0.2;

  shoreLineTop = max(-10, 0.01);

  float noise = amplitude * pnoise(frequency * gsNormal, vec3(20.0));

  // Sandy shores
  finalColor = mix(sandColor, surfaceColor, smoothstep(0.0, shoreLineTop, noise));

  // Snow on peaks
  finalColor = mix(finalColor, snowColor, smoothstep(avgTemp, avgTemp+7.0, noise));

  // Low freq noise

  ambient = ka * finalColor;
  diffuse = kd * finalColor * max(0.0, df);

  finalColor = diffuse + ambient;

  outColor = vec4(finalColor, 1.0);
//
//  // Perform perspective division to get the actual texture position
//  vec4 shadowCoordinateWdivide = lightSourceCoord / lightSourceCoord.w;
//
//  // Used to lower moire' pattern and self-shadowing
//  // The optimal value here will vary with different GPU's depending on their Z buffer resolution.
//  shadowCoordinateWdivide.z -= 0.002; // 0.0005;
//
//  float shadow = 1.0;
//  float dx, dy;
//  for (dy = -3.5 ; dy <= 3.5 ; dy+=1.0)
//    for (dx = -3.5 ; dx <= 3.5 ; dx+=1.0)
//    {
//      float distanceFromLight = texture(textureUnit, shadowCoordinateWdivide.st + vec2(dx, dy)/250.0).x;
//      distanceFromLight = (distanceFromLight-0.5) * 2.0;
//
//      if (lightSourceCoord.w > 0.0)
//        if (distanceFromLight < shadowCoordinateWdivide.z) // shadow
//          shadow -= 0.5/64; // = 0.5 shadow if total shadow (for 64 samples)
//    }
//    outColor = vec4(shadow * shade) * planetColor;
}

