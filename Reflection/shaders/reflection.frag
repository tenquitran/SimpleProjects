#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(binding = 3) uniform samplerCube cubeMapTex;

struct LightInfo
{
	// The first 3 fields are the intensities of the light components.
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 position;
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo Material;

in vec3 vPos;
in vec3 vNormal;
in vec3 reflectedRay;

out vec4 outColor;

void main()
{
	vec3 n = normalize(vNormal);
	vec3 s = normalize(vec3(Light.position) - vPos);
	vec3 v = normalize(vec3(-vPos));
	vec3 r = reflect(-s, n);

	vec3 ambient = Light.ambient * Material.ambient;
	vec3 diffuse = Light.diffuse * Material.diffuse * max(dot(s, n), 0.0);
	vec3 specular = Light.specular * Material.specular * pow(max(dot(r, v), 0.0), Material.shininess);

	vec4 phongColor = vec4(ambient + diffuse + specular, 1.0);

	vec4 cubemapTexColor = texture(cubeMapTex, reflectedRay);

	// TODO: temp
	outColor = mix(phongColor, cubemapTexColor, 0.5);

	// TODO: hard-coded color
	//outColor = vec4(0.0, 0.0, 1.0, 1.0);
}
