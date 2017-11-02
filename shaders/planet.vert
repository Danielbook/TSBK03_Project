#version 150
uniform float time;

in vec3 inPosition;
in vec3 inNormal;
in float pnoise; //pnoise in is a float, cant be used as a function below
in float colorPnoise;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

out vec3 vNormal;
out vec3 pos;
out float noise;
out float colorNoise;

void main() {
	//noise = 10.0*pnoise(15.0*inPosition+vec3(0.2, 0.34, 0.52), vec3(20.0));
	noise = pnoise;
	colorNoise = colorPnoise;

	// Apply elevation in normal
    pos = inPosition + inNormal;

    vNormal = inNormal;

    gl_Position = projectionMatrix * modelviewMatrix * vec4( pos, 1.0 );
}
