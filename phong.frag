#version 150

// Simplified Phong: No materials, only one, hard coded light source
// and no ambient


in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)
in float outPnoise;
in float outColorPnoise;

out vec4 outColor;
out float pnoise;
out float colorPnoise;

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You will usually give light in world coordinates.
	float diffuse, specular, shade;

	//Pass Noise to next shader
	pnoise = outPnoise;
    colorPnoise = outColorPnoise;

	//height*pnoise(freq*position+randVec, vec3(70.0));

	// Diffuse
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light
	
	// Specular
	vec3 r = reflect(-light, normalize(exNormal));
	vec3 v = normalize(-exSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = 1.0 * pow(specular, 150.0);
	specular = max(specular, 0.0);
	shade = 0.7*diffuse + 1.0*specular;
	outColor = vec4(shade, shade, shade, 1.0);
}
