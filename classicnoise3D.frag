#version 150

in float outPnoise;

out float inPnoise;
out float pnoise;
out vec4 outColor;

void main(void) {

    inPnoise = outPnoise;
    pnoise = outPnoise;
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
