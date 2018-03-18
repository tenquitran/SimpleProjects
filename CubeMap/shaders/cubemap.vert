#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec3 position;

out vec3 reflectedRay;

layout(location = 0) uniform mat4 Model;
layout(location = 1) uniform mat4 View;
layout(location = 2) uniform mat4 Projection;

void main()
{
	reflectedRay = position;

	gl_Position = Projection * View * Model * vec4(position, 1.0);
}
