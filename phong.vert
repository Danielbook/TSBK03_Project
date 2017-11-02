#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in  float inPnoise;
in  float inColorPnoise;

out vec3 exNormal; // Phong
out vec3 exSurface; // Phong (specular)
out float outPnoise;
out float outColorPnoise;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	exNormal = inverse(transpose(mat3(modelviewMatrix))) * inNormal; // Phong, normal transformation

	exSurface = vec3(modelviewMatrix * vec4(inPosition, 1.0)); // Don't include projection here - we only want to go to view coordinates

    //Passing on noise
    outPnoise = inPnoise;
    outColorPnoise = inColorPnoise;

	gl_Position = projectionMatrix * modelviewMatrix * vec4(inPosition, 1.0); // This should include projection
}
