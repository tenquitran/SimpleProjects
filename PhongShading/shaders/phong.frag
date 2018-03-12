#version 440 core
#pragma debug(on)
#pragma optimize(off)

//in vec4 colorFrag;

out vec4 outColor;

void main()
{
	// TODO: hard-coded color
	outColor = vec4(0.0, 0.4, 0.3, 1.0);

	//outColor = colorFrag;
}
