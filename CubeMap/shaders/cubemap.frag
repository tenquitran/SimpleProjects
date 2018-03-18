#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(binding = 3) uniform samplerCube cubeMapTex;

in vec3 reflectedRay;

out vec4 outColor;

void main()
{
	outColor = texture(cubeMapTex, reflectedRay);
}
