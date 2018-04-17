#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec3 position;

out vec3 reflectedRay;

layout(location = 0) uniform mat4 MVP;

void main()
{
	reflectedRay = position;

	gl_Position = MVP * vec4(position, 1.0);
}
