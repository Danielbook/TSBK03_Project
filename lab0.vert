/*
 * random comment here
 * makes syntax highlight appaer
 * colors like springs sprouts
 */

#version 150

in  vec3  in_Position;
in  vec3  in_Normal;
in  vec2  in_TexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 vNormal;
out vec3 vPos;

void main(void)
{
	vPos = in_Position;
	// Recalculate vertex normal
	vec4 oNormal = projectionMatrix * viewMatrix * vec4(in_Normal, 1.0);
	
	// Convert to vec3 
	vNormal = normalize(vec3(oNormal.x, oNormal.y, oNormal.z));

	gl_Position=projectionMatrix*viewMatrix*vec4(in_Position, 1.0);
}
