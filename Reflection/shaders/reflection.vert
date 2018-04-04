#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform mat4 ModelView;
layout(location = 2) uniform mat3 Normal;

out vec3 vPos;
out vec3 vNormal;
out vec3 reflectedRay;

void main()
{
	reflectedRay = position;

	vNormal = normalize(Normal * normal);

	vPos = vec3(ModelView * vec4(position, 1.0));

	gl_Position = MVP * vec4(position, 1.0);
}
