#version 150

in vec3 inPosition;
in vec3 inNormal;
in float inPnoise; //pnoise in is a float, cant be used as a function below
in float inColorPnoise;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

out vec3 vNormal;
out vec3 pos;
out float noise;
out float colorNoise;

void main() {
//	noise = 150.0 * pnoise(0.0001 * (time) * vec3(0.9, 0.26, 0.56) * inPosition, vec3(200.0));

    noise = inPnoise;
    colorNoise = inColorPnoise;

	// Apply elevation in normal 
    pos = inPosition + noise * inNormal;

    vNormal = inNormal;

    gl_Position = projectionMatrix * modelviewMatrix * vec4( pos, 1.0 );
}