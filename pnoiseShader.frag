#version 150

in float outPnoise;
in float outColorPnoise;

out float inPnoise;
out float inColorPnoise;

out vec4 outColor;

void main(void) {

    inPnoise = outPnoise;
    inColorPnoise = outColorPnoise;

	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
