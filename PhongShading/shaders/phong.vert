#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec4 position;
//layout(location = 1) in vec3 color;

uniform mat4 mvp;

//out vec4 colorFrag;

void main()
{
	gl_Position = mvp * position;

	//colorFrag = vec4(color, 1.0);
}
