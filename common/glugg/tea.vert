#version 150

in  vec3 in_Position;
in  vec3 in_Normal;
in  vec2 in_TexCoord;
uniform vec4 pos;
uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;
out float shade;
out vec2 texCoord;

void main(void)
{
	mat3 n = mat3(modelviewMatrix);
	vec3 normal = n * normalize(in_Normal);
	vec3 l = vec3(n * in_Position - vec3(pos)); // vertex to view
	l = normalize(l);
	shade = -dot(normal, l);
	shade = dot(normal, normalize(vec3(pos))) + 0.2;
//	shade = 1.0; // normal.z; // dot(normal, l);
//	shade = pos.y;
//	shade = in_Normal.y;
	gl_Position = projectionMatrix * modelviewMatrix * vec4(in_Position, 1.0);
	texCoord = in_TexCoord;
}
