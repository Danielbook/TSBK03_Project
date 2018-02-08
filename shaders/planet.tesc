#version 410 core

layout(vertices = 4) out;

in vec3 vNormal[]; // From vertex shader

uniform int TessLevelInner; // Sent from main program
uniform int TessLevelOuter1;
uniform int TessLevelOuter2;
uniform int TessLevelOuter3;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform float lod_factor;

out vec3 tcNormal[]; // Output of TC

#define ID gl_InvocationID

bool offscreen(vec4 vertex) {
  if(vertex.z < -0.5) {
    return true;
  }
  bvec2 lesser = lessThan(vertex.xy, vec2(-1.7));
  bvec2 greater = greaterThan(vertex.xy, vec2(1.7));

  if(any(lesser)) {
    return any(lesser);
  }
  else if(any(greater)){
    return any(greater);
  }
//  return any(lesser || greater);
return false;
}

vec4 project(vec4 vertex) {
  vec4 result = projectionMatrix * modelViewMatrix * vertex;
  result /= result.w;
  return result;
}

vec2 screen_space(vec4 vertex) {
  float screen_size = 1000;
  return(clamp(vertex.xy, -1.3, 1.3)+1) * (screen_size*0.5);
}

float level(vec2 v0, vec2 v1) {
  return clamp(distance(v0, v1) / lod_factor, 1, 64);
}

void main()
{
  if(ID == 0) {
        vec4 v0 = project(gl_in[0].gl_Position);
        vec4 v1 = project(gl_in[1].gl_Position);
        vec4 v2 = project(gl_in[2].gl_Position);
        vec4 v3 = project(gl_in[3].gl_Position);

        if(all(bvec3(offscreen(v0), offscreen(v1), offscreen(v2)))) {
          gl_TessLevelInner[0] = 0;
          gl_TessLevelOuter[0] = 0;
          gl_TessLevelOuter[1] = 0;
          gl_TessLevelOuter[2] = 0;
        }
        else {
          vec2 ss0 = screen_space(v0);
          vec2 ss1 = screen_space(v1);
          vec2 ss2 = screen_space(v2);
          vec2 ss3 = screen_space(v3);

          float e0 = level(ss1, ss2);
          float e1 = level(ss0, ss1);
          float e2 = level(ss3, ss0);
          float e3 = level(ss2, ss3);

          gl_TessLevelInner[0] = mix(e1, e2, 0.5);
          gl_TessLevelInner[1] = mix(e0, e3, 0.5);
          gl_TessLevelOuter[0] = e0;
          gl_TessLevelOuter[1] = e1;
          gl_TessLevelOuter[2] = e2;
          gl_TessLevelOuter[3] = e3;
        }
  }
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  tcNormal[ID] = vNormal[ID];

//  gl_TessLevelInner[0] = TessLevelInner; // How often the inside of a patch is divided
//  gl_TessLevelOuter[0] = TessLevelOuter1; // How often an edge is divided
//  gl_TessLevelOuter[1] = TessLevelOuter2;
//  gl_TessLevelOuter[2] = TessLevelOuter3;
}


